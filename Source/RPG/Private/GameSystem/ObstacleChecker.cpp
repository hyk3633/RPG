
#include "GameSystem/ObstacleChecker.h"
#include "DataAsset/MapNavDataAsset.h"
#include "Components/BoxComponent.h"
#include "Components/BillBoardComponent.h"
#include "../RPG.h"
#include "AssetRegistry/AssetRegistryModule.h"

AObstacleChecker::AObstacleChecker()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BillBoard = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillBoard"));
	BillBoard->SetupAttachment(RootComponent);

	AssetPath = TEXT("/Game/_Assets/DataAsset/");
}

void AObstacleChecker::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (CheckAssetValidity() == false)
		{
			InitMapSpecification();
			InitFieldLocations();
			bStartCheck = true;
		}
	}
}

bool AObstacleChecker::CheckAssetValidity()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(AssetPath + AssetName + TEXT(".") + AssetName);

	if (AssetData.IsValid())
	{
		UMapNavDataAsset* Asset = Cast<UMapNavDataAsset>(AssetData.GetAsset());
		PLOG(TEXT("AssetData [ %s ] is already exist."), *AssetName);
		return true;
	}
	else
	{
		PLOG(TEXT("AssetData [ %s ] is not exist."), *AssetName);
	}
	return false;
}

void AObstacleChecker::InitMapSpecification()
{
	GetActorBounds(true, Origin, Extent);

	GridWidthSize = FMath::TruncToInt((Extent.X * 2.f) / (float)GridDist) + 0.5f;
	GridLengthSize = FMath::TruncToInt((Extent.Y * 2.f) / (float)GridDist) + 0.5f;
	TotalSize = GridWidthSize * GridLengthSize;

	WorldOffsetX = ((GridWidthSize * GridDist) / 2.f) - (GridDist / 2.f);
	WorldOffsetY = ((GridLengthSize * GridDist) / 2.f) - (GridDist / 2.f);
}

void AObstacleChecker::InitFieldLocations()
{
	FieldLocations.Init(FPos(), TotalSize);
	IsMovableArr.Init(false, TotalSize);
	for (int i = 0; i < GridLengthSize; i++)
	{
		for (int j = 0; j < GridWidthSize; j++)
		{
			int32 Y = Origin.Y + ((GridDist * i) - FMath::TruncToInt(WorldOffsetY));
			int32 X = Origin.X + ((GridDist * j) - FMath::TruncToInt(WorldOffsetX));

			FieldLocations[i * GridLengthSize + j] = FPos(Y, X);
		}
	}
}

void AObstacleChecker::DrawGridPointInBox()
{
	if (FieldLocations.Num() < GridLengthSize * GridWidthSize) return;

	for (int i = 0; i < GridLengthSize; i++)
	{
		for (int j = 0; j < GridWidthSize; j++)
		{
			int32 X = FieldLocations[i * GridLengthSize + GridWidthSize].X;
			int32 Y = FieldLocations[i * GridLengthSize + GridWidthSize].Y;

			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Green, true);
		}
	}
}

void AObstacleChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAllowObstacleCheck && bStartCheck && HasAuthority())
	{
		CheckObstacleSequentially(DeltaTime);
	}
}

void AObstacleChecker::CheckObstacleSequentially(float DeltaTime)
{
	CumulatedTime += DeltaTime;
	if (CumulatedTime >= 0.1f)
	{
		CheckObstacle();
		CumulatedTime = 0.f;
	}
}

void AObstacleChecker::CheckObstacle()
{
	int32 Count = 0;
	FHitResult Hit;
	while (Count++ < 100 && LastIdx < TotalSize)
	{
		GetWorld()->LineTraceSingleByChannel
		(
			Hit,
			FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, -1000),
			FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 1000),
			ECC_ObstacleCheck
		);

		IsMovableArr[LastIdx] = !Hit.bBlockingHit;

		if (Hit.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 10), 7.5f, FColor::Red, true);
			DrawDebugLine(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, -1000), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 1000), FColor::Blue, false, 3.f, 0, 1.5f);
		}
		else
		{
			DrawDebugPoint(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 10), 7.5f, FColor::Green, true);
		}

		LastIdx++;
	}

	if (LastIdx >= TotalSize)
	{
		CreateMapNavDataAsset();
	}
}

void AObstacleChecker::CreateMapNavDataAsset()
{
	if (AssetName.Len() == 0)
	{
		ELOG(TEXT("No asset name designated!"));
		return;
	}

	AssetPath += AssetName;

	UPackage* Package = CreatePackage(nullptr, *AssetPath);
	UMapNavDataAsset* NewDataAsset = NewObject<UMapNavDataAsset>(Package, UMapNavDataAsset::StaticClass(), *AssetName, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

	NewDataAsset->NavOrigin = Origin;
	NewDataAsset->FieldLocations = FieldLocations;
	NewDataAsset->IsMovableArr = IsMovableArr;
	NewDataAsset->GridDist = GridDist;
	NewDataAsset->GridWidthSize = GridWidthSize;
	NewDataAsset->GridLengthSize = GridLengthSize;
	NewDataAsset->WorldOffsetX = WorldOffsetX;
	NewDataAsset->WorldOffsetY = WorldOffsetY;

	FAssetRegistryModule::AssetCreated(NewDataAsset);
	NewDataAsset->MarkPackageDirty();

	FString FilePath = FString::Printf(TEXT("%s%s%s"), *AssetPath, *AssetName, *FPackageName::GetAssetPackageExtension());
	bool IsSuccess = UPackage::SavePackage(Package, NewDataAsset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);
	if (IsSuccess)
	{
		WLOG(TEXT("DataAsset [ %s ] successfully created!"));
	}
	else
	{
		WLOG(TEXT("Failed to create DataAsset [ %s ]!"));
	}

	bAllowObstacleCheck = false;
	bStartCheck = false;
}



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

	BiasX = FMath::TruncToInt(((GridWidthSize * GridDist) / 2.f) - (GridDist / 2.f));
	BiasY = FMath::TruncToInt(((GridLengthSize * GridDist) / 2.f) - (GridDist / 2.f));
}

void AObstacleChecker::InitFieldLocations()
{
	FieldLocations.Init(FPos(), TotalSize);
	IsMovableArr.Init(false, TotalSize);
	ExtraCost.Init(0, TotalSize);
	BlockedGrids.Reserve(TotalSize);
	for (int i = 0; i < GridLengthSize; i++)
	{
		for (int j = 0; j < GridWidthSize; j++)
		{
			int32 Y = Origin.Y + ((GridDist * i) - BiasY);
			int32 X = Origin.X + ((GridDist * j) - BiasX);

			FieldLocations[i * GridLengthSize + j] = FPos(Y, X);
		}
	}
}

void AObstacleChecker::DrawGridPointInBox()
{
	int32 Count = 0;
	while (Count < TotalSize)
	{
		int32 X = FieldLocations[Count].X;
		int32 Y = FieldLocations[Count].Y;
		DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Green, true);
		Count++;
	}
}

void AObstacleChecker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (bAllowObstacleCheck && bStartCheck)
		{
			CheckObstacleSequentially(DeltaTime);
		}
		else if (bStartGiveScore)
		{
			GiveExtraScoreToGrid(DeltaTime);
		}
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
			ExtraCost[LastIdx] = ObstacleCost;
			BlockedGrids.Add(LastIdx);
		}
		else
		{
			DrawDebugPoint(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 10), 7.5f, FColor::Green, true);
		}

		LastIdx++;
	}

	if (LastIdx >= TotalSize)
	{
		bAllowObstacleCheck = false;
		bStartCheck = false;
		bStartGiveScore = true;
		CumulatedTime = 0.1f;
		LastIdx = 0;
		BlockedSize = BlockedGrids.Num();
	}
}

void AObstacleChecker::GiveExtraScoreToGrid(float DeltaTime)
{
	CumulatedTime += DeltaTime;
	if (CumulatedTime >= 0.1f)
	{
		int8 Count = 0;
		while (Count++ < 100 && LastIdx < BlockedSize)
		{
			BFS(BlockedGrids[LastIdx++]);
		}

		CumulatedTime = 0.f;

		if (LastIdx >= BlockedSize)
		{
			bStartGiveScore = false;
			CreateMapNavDataAsset();
		}
	}
}

void AObstacleChecker::BFS(int32 GridIdx)
{
	TArray<int32> NextGrid;
	NextGrid.Add(GridIdx);
	int8 CurrentCost = ObstacleCost - 2;
	while (!NextGrid.IsEmpty() && CurrentCost >= 4)
	{
		TArray<int32> TempArr;
		for (int32 Layer = 0; Layer < NextGrid.Num(); Layer++)
		{
			int32 CY = NextGrid[Layer] / GridLengthSize, CX = NextGrid[Layer] % GridLengthSize;
			for (int8 Idx = 0; Idx < 8; Idx++)
			{
				int32 NY = CY + Front[Idx].Y, NX = CX + Front[Idx].X;
				if (NY < 0 || NY >= GridLengthSize || NX < 0 || NX >= GridWidthSize) continue;
				
				int32 NextIdx = NY * GridLengthSize + NX;
				if (ExtraCost[NextIdx] >= CurrentCost) continue;
				
				ExtraCost[NextIdx] = CurrentCost;
				if (CurrentCost > 4) TempArr.Add(NextIdx);
				if (CurrentCost >= 8)
				{
					DrawDebugPoint(GetWorld(), FVector(FieldLocations[NextIdx].X, FieldLocations[NextIdx].Y, 15), 7.5f, FColor::Orange, true);
				}
				else
				{
					DrawDebugPoint(GetWorld(), FVector(FieldLocations[NextIdx].X, FieldLocations[NextIdx].Y, 15), 7.5f, FColor::Yellow, true);
				}	
			}
		}
		NextGrid = TempArr;
		CurrentCost -= 2;
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
	UMapNavDataAsset* NewDataAsset = NewObject<UMapNavDataAsset>
		(
			Package, 
			UMapNavDataAsset::StaticClass(), 
			*AssetName, 
			EObjectFlags::RF_Public | EObjectFlags::RF_Standalone
		);

	NewDataAsset->NavOrigin = Origin;
	NewDataAsset->FieldLocations = FieldLocations;
	NewDataAsset->IsMovableArr = IsMovableArr;
	NewDataAsset->GridDist = GridDist;
	NewDataAsset->GridWidthSize = GridWidthSize;
	NewDataAsset->GridLengthSize = GridLengthSize;
	NewDataAsset->BiasX = BiasX;
	NewDataAsset->BiasY = BiasY;
	NewDataAsset->ExtraCost = ExtraCost;

	FAssetRegistryModule::AssetCreated(NewDataAsset);
	NewDataAsset->MarkPackageDirty();

	FString FilePath = FString::Printf(TEXT("%s%s%s"), *AssetPath, *AssetName, *FPackageName::GetAssetPackageExtension());
	bool IsSuccess = UPackage::SavePackage(Package, NewDataAsset, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *FilePath);
	if (IsSuccess)
	{
		PLOG(TEXT("DataAsset [ %s ] successfully created!"), *AssetName);
	}
	else
	{
		PLOG(TEXT("Failed to create DataAsset [ %s ]!"), *AssetName);
	}
}




#include "GameSystem/ObstacleChecker.h"
#include "Components/BoxComponent.h"
#include "Components/BillBoardComponent.h"
#include "../RPG.h"
#include <cstdlib>

AObstacleChecker::AObstacleChecker()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BillBoard = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillBoard"));
	BillBoard->SetupAttachment(RootComponent);
}

void AObstacleChecker::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		InitMapSpecification();

		if (LoadMapData())
		{
			bAllowObstacleCheck = false;
		}
		else
		{
			InitFieldLocations();
			bStartCheck = true;
		}
	}
}

void AObstacleChecker::InitMapSpecification()
{
	GetActorBounds(true, Origin, Extent);

	Start = FVector(Origin.X + Extent.X, Origin.Y - Extent.Y, 10);

	GridDist = 25;

	GridWidthSize = FMath::TruncToInt((Extent.X * 2.f) / (float)GridDist) + 0.5f;
	GridLengthSize = FMath::TruncToInt((Extent.Y * 2.f) / (float)GridDist) + 0.5f;
	TotalSize = GridWidthSize * GridLengthSize;

	WorldOffsetX = ((GridWidthSize * GridDist) / 2.f) - (GridDist / 2.f);
	WorldOffsetY = ((GridLengthSize * GridDist) / 2.f) - (GridDist / 2.f);
}

bool AObstacleChecker::LoadMapData()
{
	FString FilePath = FPaths::ProjectDir() + TEXT("MapData/") + FileName;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if(PlatformFile.FileExists(*FilePath))
	{
		WLOG(TEXT("Success to read file!"));

		FString Content;
		ReadFile(FilePath, Content);

		const int32 DataLength = Content.Len();

		for (int i = 0; i < GridLengthSize; i++)
		{
			for (int j = 0; j < GridWidthSize; j++)
			{
				int32 X = Origin.X + ((GridDist * j) - FMath::TruncToInt(WorldOffsetX));
				int32 Y = Origin.Y + ((GridDist * i) - FMath::TruncToInt(WorldOffsetY));

				if (DataLength <= i * GridLengthSize + j)
				{
					ELOG(TEXT("Accessible string bound exceeded!"));
					return true;
				}

				if (Content[i * GridLengthSize + j] - TEXT('0'))
				{
					DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Red, true);
				}
				else
				{
					DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 5.f, FColor::Green, true);
				}
			}
		}

		return true;
	}
	else
	{
		ELOG(TEXT("Failed to read the file!"));
		return false;
	}
}

void AObstacleChecker::InitFieldLocations()
{
	FieldLocations.Init(FPos(), TotalSize);
	ObstaclesOrNot.Init(false, TotalSize);
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

	if (bAllowObstacleCheck && bStartCheck && HasAuthority() == false)
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

		DrawDebugLine(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, -1000), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 1000), FColor::Blue, false, 3.f, 0, 1.5f);

		ObstacleCheckData.AppendInt(Hit.bBlockingHit);
		ObstaclesOrNot[LastIdx] = Hit.bBlockingHit;

		if (Hit.bBlockingHit)
		{
			DrawDebugPoint(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 10), 7.5f, FColor::Red, true);
		}
		else
		{
			DrawDebugPoint(GetWorld(), FVector(FieldLocations[LastIdx].X, FieldLocations[LastIdx].Y, 10), 7.5f, FColor::Green, true);
		}

		LastIdx++;
	}

	if (LastIdx >= TotalSize)
	{
		if (FileName.Len() == 0)
		{
			ELOG(TEXT("No file name designated!"));
			return;
		}

		FString FilePath = FPaths::ProjectDir() + TEXT("MapData/") + FileName;

		WriteToFile(FilePath, ObstacleCheckData);

		bAllowObstacleCheck = false;
		bStartCheck = false;
	}
}

void AObstacleChecker::WriteToFile(const FString& FilePath, const FString& Content)
{
	if (FFileHelper::SaveStringToFile(Content, *FilePath))
	{
		WLOG(TEXT("The Data File is Successfully Saved!"));
	}
}

bool AObstacleChecker::ReadFile(const FString& FilePath, FString& OutContent)
{
	return FFileHelper::LoadFileToString(OutContent, *FilePath);
}


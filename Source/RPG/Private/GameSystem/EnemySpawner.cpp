
#include "GameSystem/EnemySpawner.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/EnemyPooler.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "DataAsset/MapNavDataAsset.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Components/BoxComponent.h"
#include "AssetRegistry/AssetRegistryModule.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	SetRootComponent(AreaBox);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionResponseToChannel(ECC_PlayerBody, ECR_Overlap);

	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));
}

void AEnemySpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority() == false)
	{
		AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		InitFlowField();

		AreaBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnAreaBoxBeginOverlap);
		AreaBox->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnAreaBoxEndOverlap);
	}
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false) return;
	
	AEnemyPooler* EnemyPooler = GetWorld()->SpawnActor<AEnemyPooler>(FVector::ZeroVector, FRotator::ZeroRotator);
	EnemyPooler->CreatePool(4, EEnemyType::EET_Skeleton);
	EnemyPoolerMap.Add(StaticCast<int32>(EEnemyType::EET_Skeleton), EnemyPooler);

	for (ARPGBaseEnemyCharacter* Enemy : EnemyPooler->GetEnemyArr())
	{
		Enemy->DOnDeath.AddUFunction(this, FName("EnemyRespawnDelay"));
	}

	ARPGBaseEnemyCharacter* Enemy = EnemyPooler->GetPooledEnemy();
	if (Enemy)
	{
		Enemy->SetActorLocation(FVector(270, -150, 0));
		Enemy->SetActorRotation(FRotator(0, 180, 0));
		Enemy->SetSpawner(this);
		Enemy->ActivateEnemy();
	}

	ARPGBaseEnemyCharacter* Enemy2 = EnemyPooler->GetPooledEnemy();
	if (Enemy2)
	{
		Enemy2->SetActorLocation(FVector(0, -500, 0));
		Enemy2->SetActorRotation(FRotator(0, 180, 0));
		Enemy2->SetSpawner(this);
		Enemy2->ActivateEnemy();
	}

	EnemiesInArea.Add(Enemy);
	EnemiesInArea.Add(Enemy2);
}

void AEnemySpawner::InitFlowField()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*FlowFieldDataReference);
	if (AssetData.IsValid())
	{
		UMapNavDataAsset* FlowFieldDA = Cast<UMapNavDataAsset>(AssetData.GetAsset());

		OriginLocation = FlowFieldDA->NavOrigin;

		GridDist = FlowFieldDA->GridDist;
		GridWidth = FlowFieldDA->GridWidthSize;
		GridLength = FlowFieldDA->GridLengthSize;
		TotalSize = GridWidth * GridLength;

		BiasY = FlowFieldDA->BiasY;
		BiasX = FlowFieldDA->BiasX;

		IsMovableArr = FlowFieldDA->IsMovableArr;
	}
}

void AEnemySpawner::OnAreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter)
	{
		PlayersInArea.Add(PlayerCharacter);
		TargetsFlowVectors.Add(PlayerCharacter, FFlowVector(TotalSize));
	}
}

void AEnemySpawner::OnAreaBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter)
	{
		if (PlayersInArea.Find(PlayerCharacter) != INDEX_NONE)
		{
			PlayersInArea.Remove(PlayerCharacter);
			TargetsFlowVectors.Remove(PlayerCharacter);
		}
	}
}

void AEnemySpawner::EnemyRespawnDelay()
{
	GetWorldTimerManager().SetTimer(EnemyRespawnTimer, this, &AEnemySpawner::EnemyRespawn, 5);
}

void AEnemySpawner::EnemyRespawn()
{
	const int32 Index = StaticCast<int32>(EEnemyType::EET_Skeleton);
	if (EnemyPoolerMap.Contains(Index) == false) return;

	ARPGBaseEnemyCharacter* Enemy = (*EnemyPoolerMap.Find(Index))->GetPooledEnemy();
	if (Enemy)
	{
		Enemy->SetActorLocation(FVector(0, 0, 0));
		Enemy->SetActorRotation(FRotator(0, 0, 180));
		Enemy->ActivateEnemy();
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && PlayersInArea.Num())
	{
		CumulTime += DeltaTime;
		if (CumulTime >= 0.3f)
		{
			for (ACharacter* Target : PlayersInArea)
			{
				CalculateFlowVector(Target);
			}
			CumulTime = 0.f;
		}
	}
}

FVector& AEnemySpawner::GetFlowVector(ACharacter* TargetCharacter, ACharacter* EnemyCharacter)
{
	const int32 Idx = GetConvertCurrentLocationToIndex(EnemyCharacter->GetActorLocation());
	FFlowVector* FlowVector = TargetsFlowVectors.Find(TargetCharacter);
	return FlowVector->GridFlows[Idx];
}

int32 AEnemySpawner::GetConvertCurrentLocationToIndex(const FVector& Location)
{
	int32 DX = FMath::Floor(((Location.X - OriginLocation.X + BiasX) / GridDist) + 0.5f);
	int32 DY = FMath::Floor(((Location.Y - OriginLocation.Y + BiasY) / GridDist) + 0.5f);

	return (DY * GridWidth + DX);
}

void AEnemySpawner::CalculateFlowVector(ACharacter* TargetCharacter)
{
	double start = FPlatformTime::Seconds();

	FFlowVector* FVArr = TargetsFlowVectors.Find(TargetCharacter);
	if (FVArr == nullptr) return;

	const int32 DestIdx = GetConvertCurrentLocationToIndex(TargetCharacter->GetActorLocation());
	if (DestIdx >= TotalSize) return;

	FVArr->Score.Init(-1, TotalSize);

	int32* DistScore = FVArr->Score.GetData();
	FVector* FlowVector = FVArr->GridFlows.GetData();
	if (DistScore == nullptr || FlowVector == nullptr) return;

	*(DistScore + DestIdx) = 0;

	TQueue<FPos> Next;
	Next.Enqueue(FPos(DestIdx / GridWidth, DestIdx % GridWidth));

	while (!Next.IsEmpty())
	{
		FPos CPos;
		Next.Dequeue(CPos);

		for (int8 Idx = 0; Idx < 8; Idx++)
		{
			FPos NextPos = CPos + Front[Idx];
			const int32 NextIdx = NextPos.Y * GridWidth + NextPos.X;
			if (NextPos.Y >= 0 && NextPos.Y < GridLength &&
				NextPos.X >= 0 && NextPos.X < GridWidth)
			{
				if (*(DistScore + NextIdx) != -1) continue;
				if (IsMovableArr[NextIdx] == false) continue;

				*(DistScore + NextIdx) = *(DistScore + (CPos.Y * GridWidth + CPos.X)) + 1;
				Next.Enqueue(NextPos);
			}
		}
	}

	for (int32 CY = 0; CY < GridLength; CY++)
	{
		for (int32 CX = 0; CX < GridWidth; CX++)
		{
			if (CY * GridWidth + CX == DestIdx) continue;
			bool Flag = false;
			FVector Loc = FVector(OriginLocation.X + (GridDist * CX) - BiasX, OriginLocation.Y + (GridDist * CY) - BiasY, 30);
			int32 Min = INT_MAX, Idx = 0;
			for (int8 Dir = 0; Dir < 8; Dir++)
			{
				FPos NPos = FPos(CY, CX) + Front[Dir];
				if (NPos.Y >= 0 && NPos.Y < GridLength && NPos.X >= 0 && NPos.X < GridWidth)
				{
					int32 NextIdx = NPos.Y * GridWidth + NPos.X;
					if (Min > *(DistScore+NextIdx) && *(DistScore+NextIdx) >= 0)
					{
						Min = *(DistScore + NextIdx);
						Idx = Dir;
					}
				}
			}
			FVector Loc2 = FVector(OriginLocation.X + (GridDist * (CX + Front[Idx].X)) - BiasX, OriginLocation.Y + (GridDist * (CY + Front[Idx].Y)) - BiasY, 30);
			*(FlowVector + (CY * GridWidth + CX)) = (Loc2 - Loc).GetSafeNormal();
		}
	}

	double end = FPlatformTime::Seconds();
	PLOG(TEXT("time : %f"), end - start);
}

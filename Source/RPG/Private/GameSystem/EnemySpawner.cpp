
#include "GameSystem/EnemySpawner.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/EnemyPooler.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Components/BoxComponent.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	AreaBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Area Box"));
	SetRootComponent(AreaBox);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionResponseToChannel(ECC_PlayerBody, ECR_Overlap);

	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false)
	{
		AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}

	AreaBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnAreaBoxBeginOverlap);
	AreaBox->OnComponentEndOverlap.AddDynamic(this, &AEnemySpawner::OnAreaBoxEndOverlap);
	
	AEnemyPooler* EnemyPooler = GetWorld()->SpawnActor<AEnemyPooler>(FVector::ZeroVector, FRotator::ZeroRotator);
	EnemyPooler->CreatePool(4, EEnemyType::EET_SkeletonArcher);
	EnemyPoolerMap.Add(StaticCast<int32>(EEnemyType::EET_SkeletonArcher), EnemyPooler);

	for (ARPGBaseEnemyCharacter* Enemy : EnemyPooler->GetEnemyArr())
	{
		Enemy->DOnDeath.AddUFunction(this, FName("EnemyRespawnDelay"));
	}

	ARPGBaseEnemyCharacter* Enemy = EnemyPooler->GetPooledEnemy();
	if (Enemy)
	{
		Enemy->SetActorLocation(FVector(370, 180, 0));
		Enemy->SetActorRotation(FRotator(900, 180, 0));
		Enemy->ActivateEnemy();
	}

	ARPGBaseEnemyCharacter* Enemy2 = EnemyPooler->GetPooledEnemy();
	if (Enemy2)
	{
		Enemy2->SetActorLocation(FVector(0, 200, 0));
		Enemy2->SetActorRotation(FRotator(0, 180, 0));
		Enemy2->ActivateEnemy();
	}

	EnemiesInArea.Add(Enemy);
	EnemiesInArea.Add(Enemy2);
}

void AEnemySpawner::OnAreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CF();
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter)
	{
		PlayersInArea.Add(PlayerCharacter);
		TargetAndEnemies.Add(PlayerCharacter, FEnemiesOfTarget());

		CalculateDistanceBetweenPlayersAndEnemies();
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
			TargetAndEnemies.Remove(PlayerCharacter);

			if (!PlayersInArea.Num())
			{
				GetWorldTimerManager().ClearTimer(GettingPathTimer);
			}
		}
	}
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::CalculateDistanceBetweenPlayersAndEnemies()
{
	for (ARPGBaseEnemyCharacter* Enemy : EnemiesInArea)
	{
		if (!IsValid(Enemy)) continue;
		CF();
		ACharacter* TargetPlayer = nullptr;
		float MinDistance = Enemy->GetAttackDistance();
		for (ACharacter* Player : PlayersInArea)
		{
			if (!IsValid(Player)) continue;

			const float DistanceTo = Enemy->GetDistanceTo(Player);
			if (DistanceTo <= MinDistance)
			{
				MinDistance = DistanceTo;
				TargetPlayer = Player;
			}
		}

		if (TargetPlayer)
		{
			FEnemiesOfTarget* EnemiesOfTarget = TargetAndEnemies.Find(TargetPlayer);
			if(EnemiesOfTarget) EnemiesOfTarget->Enemies.Add(FEnemyAndDistance(Enemy, MinDistance));
		}
	}
	GetWorldTimerManager().SetTimer(GettingPathTimer, this, &AEnemySpawner::GetEnemiesPathToPlayers, 0.1f, true);
}

void AEnemySpawner::GetEnemiesPathToPlayers()
{
	ACharacter* Player = PlayersInArea[PathOrderIdx];
	if (TargetAndEnemies.Find(Player))
	{
		TargetAndEnemies[Player].Enemies.Sort();
		for (const FEnemyAndDistance& Pair : TargetAndEnemies[Player].Enemies)
		{
			// TODO : 경로 받아오기 함수
			PLOG(TEXT("%s : %s , %f"), *Player->GetName(), *Pair.Enemy->GetName(), Pair.Distance);
		}
		TargetAndEnemies[Player].Enemies.Empty();
	}
	PathOrderIdx++;
	if (PathOrderIdx == PlayersInArea.Num())
	{
		PathOrderIdx = 0;
		GetWorldTimerManager().ClearTimer(GettingPathTimer);
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
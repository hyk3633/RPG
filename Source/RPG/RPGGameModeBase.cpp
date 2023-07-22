

#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "GameSystem/EnemyPooler.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));
	WorldGridManager->InitWorldGrid(80, 25);

	ItemSpawnManager = CreateDefaultSubobject<UItemSpawnManagerComponent>(TEXT("Item Spawn Manager"));

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyInfo.DT_EnemyInfo"));
	if (Obj_EnemyInfoDataTable.Succeeded()) EnemyInfoDataTable = Obj_EnemyInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyAssets.DT_EnemyAssets"));
	if (Obj_EnemyAssetsDataTable.Succeeded()) EnemyAssetsDataTable = Obj_EnemyAssetsDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileInfo.DT_ProjectileInfo"));
	if (Obj_ProjectileInfoDataTable.Succeeded()) ProjectileInfoDataTable = Obj_ProjectileInfoDataTable.Object;

}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	AEnemyPooler* EnemyPooler = GetWorld()->SpawnActor<AEnemyPooler>(FVector::ZeroVector, FRotator::ZeroRotator);
	EnemyPooler->CreatePool(2, EEnemyType::EET_SkeletonArcher);
	EnemyPoolerMap.Add(StaticCast<int32>(EEnemyType::EET_SkeletonArcher), EnemyPooler);

	for (ARPGBaseEnemyCharacter* Enemy : EnemyPooler->GetEnemyArr())
	{
		Enemy->DOnDeath.AddUFunction(this, FName("EnemyRespawnDelay"));
	}

	ARPGBaseEnemyCharacter* Enemy = EnemyPooler->GetPooledEnemy();
	if (Enemy)
	{
		Enemy->SetActorLocation(FVector(0, 0, 0));
		Enemy->SetActorRotation(FRotator(0, 180, 0));
		Enemy->ActivateEnemy();
	}
}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY)
{
	WorldGridManager->AStar(Start, Dest, PathToDestX, PathToDestY);
}

void ARPGGameModeBase::SpawnItems(const FVector& Location)
{
	ItemSpawnManager->SpawnItems(Location);
}

void ARPGGameModeBase::DropItem(const FItemInfo& Info, const FVector& Location)
{
	ItemSpawnManager->DropItem(Info, Location);
}

FEnemyInfo* ARPGGameModeBase::GetEnemyInfo(const EEnemyType Type)
{
	if (EnemyInfoDataTable)
	{
		FEnemyInfo* Data = EnemyInfoDataTable->FindRow<FEnemyInfo>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

FEnemyAssets* ARPGGameModeBase::GetEnemyAssets(const EEnemyType Type)
{
	if (EnemyAssetsDataTable)
	{
		FEnemyAssets* Data = EnemyAssetsDataTable->FindRow<FEnemyAssets>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

FProjectileInfo* ARPGGameModeBase::GetProjectileInfo(const EProjectileType Type)
{
	if (ProjectileInfoDataTable)
	{
		FProjectileInfo* Data = ProjectileInfoDataTable->FindRow<FProjectileInfo>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

void ARPGGameModeBase::EnemyRespawnDelay()
{
	GetWorldTimerManager().SetTimer(EnemyRespawnTimer, this, &ARPGGameModeBase::EnemyRespawn, 5);
}

void ARPGGameModeBase::EnemyRespawn()
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


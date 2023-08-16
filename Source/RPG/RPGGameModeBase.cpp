

#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "GameSystem/EnemyPooler.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Boss/RPGBossEnemyCharacter.h"
#include "../RPG.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));

	ItemSpawnManager = CreateDefaultSubobject<UItemSpawnManagerComponent>(TEXT("Item Spawn Manager"));

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyInfo.DT_EnemyInfo"));
	if (Obj_EnemyInfoDataTable.Succeeded()) EnemyInfoDataTable = Obj_EnemyInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyAssets.DT_EnemyAssets"));
	if (Obj_EnemyAssetsDataTable.Succeeded()) EnemyAssetsDataTable = Obj_EnemyAssetsDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileInfo.DT_ProjectileInfo"));
	if (Obj_ProjectileInfoDataTable.Succeeded()) ProjectileInfoDataTable = Obj_ProjectileInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileAssets.DT_ProjectileAssets"));
	if (Obj_ProjectileAssetsDataTable.Succeeded()) ProjectileAssetsDataTable = Obj_ProjectileAssetsDataTable.Object;
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//AEnemyPooler* BossPooler = GetWorld()->SpawnActor<AEnemyPooler>(FVector::ZeroVector, FRotator::ZeroRotator);
	//BossPooler->CreatePool(1, EEnemyType::EET_Boss);
	//EnemyPoolerMap.Add(StaticCast<int32>(EEnemyType::EET_Boss), BossPooler);
	//BossPooler->GetEnemyArr()[0]->DOnDeath.AddUFunction(this, FName("EnemyRespawnDelay"));
	//ARPGBossEnemyCharacter* Boss = Cast<ARPGBossEnemyCharacter>(BossPooler->GetPooledEnemy());
	//if (Boss)
	//{
	//	Boss->SetActorLocation(FVector(0, 0, 0));
	//	Boss->SetActorRotation(FRotator(0, 180, 0));
	//	Boss->ActivateEnemy();
	//}
}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->AStarPlayer(Start, Dest, PathToDest);
	}
	else
	{
		ELOG(TEXT("Navigation is disable!"));
	}
}

void ARPGGameModeBase::UpdateCharacterExtraCost(int32& CoordinateY, int32& CoordinateX, const FVector& Location)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->UpdateCharacterExtraCost(CoordinateY, CoordinateX, Location);
	}
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

FProjectileAssets* ARPGGameModeBase::GetProjectileAssets(const EProjectileType Type)
{
	if (ProjectileAssetsDataTable)
	{
		FProjectileAssets* Data = ProjectileAssetsDataTable->FindRow<FProjectileAssets>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}


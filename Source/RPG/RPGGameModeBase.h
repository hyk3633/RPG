
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Enums/EnemyType.h"
#include "Enums/ProjectileType.h"
#include "Structs/EnemyInfo.h"
#include "Structs/EnemyAssets.h"
#include "Structs/ProjectileInfo.h"
#include "Structs/ProjectileAssets.h"
#include "Structs/Pos.h"
#include "RPGGameModeBase.generated.h"

/**
 * 
 */

class UWorldGridManagerComponent;
class UItemSpawnManagerComponent;
class UDataTable;
class AEnemyPooler;
struct FItemInfo;

UCLASS()
class RPG_API ARPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGGameModeBase();

protected:

	virtual void BeginPlay() override;

public:

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest);

	void SpawnItems(const FVector& Location);

	void DropItem(const FItemInfo& Info, const FVector& Location);

	FEnemyInfo* GetEnemyInfo(const EEnemyType Type);

	FEnemyAssets* GetEnemyAssets(const EEnemyType Type);

	FProjectileInfo* GetProjectileInfo(const EProjectileType Type);

	FProjectileAssets* GetProjectileAssets(const EProjectileType Type);

protected:

	UFUNCTION()
	void EnemyRespawnDelay();

	void EnemyRespawn();

private:

	UPROPERTY()
	UWorldGridManagerComponent* WorldGridManager;
	
	UPROPERTY(EditAnywhere)
	UItemSpawnManagerComponent* ItemSpawnManager;

	UPROPERTY()
	UDataTable* EnemyInfoDataTable;

	UPROPERTY()
	UDataTable* EnemyAssetsDataTable;

	UPROPERTY()
	UDataTable* ProjectileInfoDataTable;

	UPROPERTY()
	UDataTable* ProjectileAssetsDataTable;

	UPROPERTY()
	TMap<int32, AEnemyPooler*> EnemyPoolerMap;

	FTimerHandle EnemyRespawnTimer;

};

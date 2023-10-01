
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Enums/EnemyType.h"
#include "Enums/ProjectileType.h"
#include "Structs/ItemInfo.h"
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
class ARPGBasePlayerCharacter;
class UDataTable;
class AEnemyPooler;

UCLASS()
class RPG_API ARPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGGameModeBase();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

protected:

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

public:

	void SpawnPlayerCharacterAndPossess(APlayerController* Player);

	FORCEINLINE UWorldGridManagerComponent* GetWorldGridManager() const { return WorldGridManager; }

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest);

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest, TArray<int32>& GridIndexArr);

	void ClearEnemiesPathCost(TArray<int32>& GridIndexArr);

	void SetGridToImpassable(const FPos& _Pos);

	void SetGridToPassable(const FPos& _Pos);

	void SpawnItems(const FVector& Location);

	void DropItem(const FItemInfo& Info, const FVector& Location);

	FEnemyInfo* GetEnemyInfo(const EEnemyType Type);

	FEnemyAssets* GetEnemyAssets(const EEnemyType Type);

	FProjectileInfo* GetProjectileInfo(const EProjectileType Type);

	FProjectileAssets* GetProjectileAssets(const EProjectileType Type);

	void DrawScore(const FVector& Location);

private:

	UPROPERTY(EditAnywhere)
	UWorldGridManagerComponent* WorldGridManager;
	
	UPROPERTY(EditAnywhere)
	UItemSpawnManagerComponent* ItemSpawnManager;

	UPROPERTY()
	TSubclassOf<ARPGBasePlayerCharacter> WarriorCharacterClass;

	UPROPERTY()
	TSubclassOf<ARPGBasePlayerCharacter> SorcererCharacterClass;

	UPROPERTY()
	UDataTable* EnemyInfoDataTable;

	UPROPERTY()
	UDataTable* EnemyAssetsDataTable;

	UPROPERTY()
	UDataTable* ProjectileInfoDataTable;

	UPROPERTY()
	UDataTable* ProjectileAssetsDataTable;
};

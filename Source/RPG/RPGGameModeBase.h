
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "Enums/EnemyType.h"
#include "RPGGameModeBase.generated.h"

/**
 * 
 */

class UWorldGridManagerComponent;
class UItemSpawnManagerComponent;
class UDataTable;
struct FItemInfo;

USTRUCT(BlueprintType)
struct FEnemyData : public FTableRowBase
{
	GENERATED_BODY()

public:

	FEnemyData() : Name(""), Stk(1.f), Def(1.f), MaxHP(100), Exp(10) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Stk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float Def;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 Exp;
};

UCLASS()
class RPG_API ARPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGGameModeBase();

protected:

	virtual void BeginPlay() override;

public:

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY);

	void SpawnItems(const FVector& Location);

	void DropItem(const FItemInfo& Info, const FVector& Location);

	FEnemyData* GetEnemyData(const EEnemyType Type);

private:

	UPROPERTY()
	UWorldGridManagerComponent* WorldGridManager;
	
	UPROPERTY(EditAnywhere)
	UItemSpawnManagerComponent* ItemSpawnManager;

	UPROPERTY()
	UDataTable* ItemDataTable;

};

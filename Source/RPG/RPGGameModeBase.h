
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGGameModeBase.generated.h"

/**
 * 
 */

class UWorldGridManagerComponent;
class UItemSpawnManagerComponent;
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

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY);

	void SpawnItems(const FVector& Location);

	void DropItem(const FItemInfo& Info, const FVector& Location);

private:

	UPROPERTY()
	UWorldGridManagerComponent* WorldGridManager;
	
	UPROPERTY(EditAnywhere)
	UItemSpawnManagerComponent* ItemSpawnManager;
};

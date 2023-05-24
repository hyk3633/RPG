
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RPGGameModeBase.generated.h"

/**
 * 
 */

class UWorldGridManagerComponent;

UCLASS()
class RPG_API ARPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGGameModeBase();

	void GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY);

protected:

	virtual void BeginPlay() override;

private:

	UWorldGridManagerComponent* WorldGridManagerComponent;
	
};

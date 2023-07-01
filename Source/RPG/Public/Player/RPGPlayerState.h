
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/ItemType.h"
#include "RPGPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	void AddItem(EItemType Type);

private:

	TArray<int32> ItemArray;

};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/ItemType.h"
#include "RPGPlayerState.generated.h"

/**
 * 
 */

class ARPGItem;

UCLASS()
class RPG_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	void AddItem(ARPGItem* PickedItem);

private:

	TArray<int32> ItemArray;

};

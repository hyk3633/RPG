
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/ItemType.h"
#include "Item/RPGItem.h"
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

	FORCEINLINE int32 GetLastItemArrayNumber() { return ItemArr.Num() - 1; }
	FORCEINLINE int32 GetCoins() const { return Coins; }
	FORCEINLINE int32 GetHealthPotionCount() const { return HealthPotionCount; }
	FORCEINLINE int32 GetManaPotionCount() const { return ManaPotionCount; }

public:

	void AddItem(ARPGItem* PickedItem);

private:

	int32 Coins = 0;

	int32 HealthPotionCount = 0;
	int32 ManaPotionCount = 0;
	TArray<FItemInfo> ItemArr;

};

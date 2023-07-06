
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

	void UseItem(const int32& ItemNum);

	void DiscardItem(const int32& ItemNum);

	FORCEINLINE int32 GetLastItemArrayNumber() { return CurrentItemUniqueNum; }
	FORCEINLINE int32 GetCoins() const { return Coins; }
	FORCEINLINE int32 GetHealthPotionCount() const { return HealthPotionCount; }
	FORCEINLINE int32 GetManaPotionCount() const { return ManaPotionCount; }
	
	FItemInfo GetItemInfo(const int32& ItemNum);

public:

	void AddItem(ARPGItem* PickedItem);

private:

	int32 Coins = 0;

	int32 HealthPotionCount = 0;

	int32 ManaPotionCount = 0;

	TMap<int32, FItemInfo> ItemMap;

	int32 CurrentItemUniqueNum = 2;

};

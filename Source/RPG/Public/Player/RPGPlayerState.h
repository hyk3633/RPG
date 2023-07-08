
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/ItemType.h"
#include "Item/RPGItem.h"
#include "RPGPlayerState.generated.h"

/**
 * 
 */

USTRUCT(Atomic)
struct FCharacterStats
{
	GENERATED_BODY()
public:

	UPROPERTY()
	float DefenseivePower;

	UPROPERTY()
	float Dexterity;

	UPROPERTY()
	int32 ExtraMP;

	UPROPERTY()
	int32 ExtraHP;

	UPROPERTY()
	float StrikingPower;

	UPROPERTY()
	float SkillPower;

	UPROPERTY()
	float AttackSpeed;

	FCharacterStats() : DefenseivePower(1), Dexterity(1), ExtraMP(0), ExtraHP(0), StrikingPower(1), SkillPower(1), AttackSpeed(1) {}
};

UCLASS()
class RPG_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void UseItem(const int32& UniqueNum);

	void EquipOrUnequipItem(const int32& UniqueNum);

	void DiscardItem(const int32& UniqueNum);

	bool GetItemInfo(const int32& UniqueNum, FItemInfo& ItemInfo);

	FORCEINLINE int32 GetLastItemArrayNumber() { return CurrentItemUniqueNum; }
	FORCEINLINE int32 GetCoins() const { return Coins; }
	FORCEINLINE int32 GetHealthPotionCount() const { return HealthPotionCount; }
	FORCEINLINE int32 GetManaPotionCount() const { return ManaPotionCount; }
	FORCEINLINE const FCharacterStats& GetItemInfo() { return CharacterStats; }

public:

	const int32 AddItem(ARPGItem* PickedItem);

private:

	int32 Coins = 0;

	int32 HealthPotionCount = 0;

	int32 ManaPotionCount = 0;

	TMap<int32, FItemInfo> ItemMap;

	int32 CurrentItemUniqueNum = 2;

	int32 EquippedArmourUniqueNum;

	int32 EquippedAccessoriesUniqueNum;

	FCharacterStats CharacterStats;
};

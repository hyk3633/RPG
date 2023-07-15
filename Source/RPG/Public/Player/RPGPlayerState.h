
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
	int32 MaxMP;

	UPROPERTY()
	int32 MaxHP;

	UPROPERTY()
	float StrikingPower;

	UPROPERTY()
	float SkillPower;

	UPROPERTY()
	float AttackSpeed;

	FCharacterStats() : DefenseivePower(1), Dexterity(1), MaxMP(0), MaxHP(0), StrikingPower(1), SkillPower(1), AttackSpeed(1) {}
	FCharacterStats& operator=(const FCharacterStats& Other)
	{
		if (this == &Other) return *this;
		DefenseivePower = Other.DefenseivePower;
		Dexterity = Other.Dexterity;
		MaxMP = Other.MaxMP;
		MaxHP = Other.MaxHP;
		StrikingPower = Other.StrikingPower;
		SkillPower = Other.SkillPower;
		AttackSpeed = Other.AttackSpeed;
		return *this;
	}
};

UCLASS()
class RPG_API ARPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void UseItem(const int32& UniqueNum);

	const bool EquipItem(const int32& UniqueNum);

	void UnequipItem(const int32& UniqueNum);

protected:

	void UpdateEquippedItemStats(const FItemInfo* Info, const bool bIsEquip);

public:

	void DiscardItem(const int32& UniqueNum);

	bool GetItemInfo(const int32& UniqueNum, FItemInfo& ItemInfo);

	FORCEINLINE int32 GetLastItemArrayNumber() { return CurrentItemUniqueNum; }
	FORCEINLINE int32 GetCoins() const { return Coins; }
	int32 GetItemCount(const int32& UniqueNum) const;
	FORCEINLINE const FCharacterStats& GetCurrentCharacterStats() { return CharacterStats; }
	FORCEINLINE const FCharacterStats& GetEquippedItemStats() { return EquippedItemStats; }
	const bool GetItemStatInfo(const int32 UniqueNum, FItemInfo& Info);
	const bool IsEquippedItem(const int32& UniqueNum) { return (UniqueNum == EquippedArmourUniqueNum || UniqueNum == EquippedAccessoriesUniqueNum); }

public:

	const int32 AddItem(ARPGItem* PickedItem);

private:

	int32 Coins = 0;

	int32 HealthPotionCount = 0;

	int32 ManaPotionCount = 0;

	TMap<int32, FItemInfo> ItemMap;

	int32 CurrentItemUniqueNum = 2;

	int32 EquippedArmourUniqueNum = -1;

	int32 EquippedAccessoriesUniqueNum = -1;

	FCharacterStats CharacterStats;

	FCharacterStats EquippedItemStats;
};

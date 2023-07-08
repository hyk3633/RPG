

#include "Player/RPGPlayerState.h"
#include "../RPG.h"

void ARPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 0번은 체력 포션, 1번은 마나 포션
}

void ARPGPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGPlayerState::UseItem(const int32& UniqueNum)
{
	if (UniqueNum)
	{
		ManaPotionCount -= 1;
	}
	else
	{
		HealthPotionCount -= 1;
	}
}

void ARPGPlayerState::EquipOrUnequipItem(const int32& UniqueNum)
{
	if (ItemMap.Contains(UniqueNum) == false) return;

	const FItemInfo* EquippedItemInfo = ItemMap.Find(UniqueNum);

	if (EquippedArmourUniqueNum == UniqueNum)
	{
		CharacterStats.DefenseivePower	= FMath::Clamp(CharacterStats.DefenseivePower	- EquippedItemInfo->ItemStatArr[0], 1.f, 10.f);
		CharacterStats.Dexterity		= FMath::Clamp(CharacterStats.Dexterity			- EquippedItemInfo->ItemStatArr[1], 1.f, 2.f);
		CharacterStats.ExtraMP			= FMath::Clamp(CharacterStats.ExtraMP			- EquippedItemInfo->ItemStatArr[2], 0, 500);
		CharacterStats.ExtraHP			= FMath::Clamp(CharacterStats.ExtraHP			- EquippedItemInfo->ItemStatArr[3], 0, 500);

		EquippedArmourUniqueNum = -1;
	}
	else if(EquippedAccessoriesUniqueNum == UniqueNum)
	{
		CharacterStats.StrikingPower	= FMath::Clamp(CharacterStats.StrikingPower		- EquippedItemInfo->ItemStatArr[0], 1.f, 10.f);
		CharacterStats.SkillPower		= FMath::Clamp(CharacterStats.SkillPower		- EquippedItemInfo->ItemStatArr[1], 1.f, 10.f);
		CharacterStats.AttackSpeed		= FMath::Clamp(CharacterStats.AttackSpeed		- EquippedItemInfo->ItemStatArr[2], 1.f, 2.f);

		EquippedAccessoriesUniqueNum = -1;
	}
	else
	{
		if (EquippedItemInfo->ItemType == EItemType::EIT_Armour)
		{
			CharacterStats.DefenseivePower	= FMath::Clamp(CharacterStats.DefenseivePower	+ EquippedItemInfo->ItemStatArr[0], 1.f, 10.f);
			CharacterStats.Dexterity		= FMath::Clamp(CharacterStats.Dexterity			+ EquippedItemInfo->ItemStatArr[1], 1.f, 2.f);
			CharacterStats.ExtraMP			= FMath::Clamp(CharacterStats.ExtraMP			+ EquippedItemInfo->ItemStatArr[2], 0, 500);
			CharacterStats.ExtraHP			= FMath::Clamp(CharacterStats.ExtraHP			+ EquippedItemInfo->ItemStatArr[3], 0, 500);

			EquippedArmourUniqueNum = UniqueNum;
		}
		else if (EquippedItemInfo->ItemType == EItemType::EIT_Accessories)
		{
			CharacterStats.StrikingPower	= FMath::Clamp(CharacterStats.StrikingPower		+ EquippedItemInfo->ItemStatArr[0], 1.f, 10.f);
			CharacterStats.SkillPower		= FMath::Clamp(CharacterStats.SkillPower		+ EquippedItemInfo->ItemStatArr[1], 1.f, 10.f);
			CharacterStats.AttackSpeed		= FMath::Clamp(CharacterStats.AttackSpeed		+ EquippedItemInfo->ItemStatArr[2], 1.f, 2.f);

			EquippedAccessoriesUniqueNum = UniqueNum;
		}
	}
}

void ARPGPlayerState::DiscardItem(const int32& UniqueNum)
{
	if (UniqueNum == 0)
	{
		HealthPotionCount -= 1;
		if (HealthPotionCount == 0)
		{
			ItemMap.Remove(UniqueNum);
		}
	}
	else if (UniqueNum == 1)
	{
		ManaPotionCount -= 1;
		if (ManaPotionCount == 0)
		{
			ItemMap.Remove(UniqueNum);
		}
	}
	else
	{
		ItemMap.Remove(UniqueNum);
	}
}

bool ARPGPlayerState::GetItemInfo(const int32& UniqueNum, FItemInfo& ItemInfo)
{
	if (ItemMap.Contains(UniqueNum))
	{
		ItemInfo = (*ItemMap.Find(UniqueNum));
		return true;
	}
	return false;
}

const int32 ARPGPlayerState::AddItem(ARPGItem* PickedItem)
{
	EItemType PickedItemType = PickedItem->GetItemInfo().ItemType;
	const int32 Idx = StaticCast<int32>(PickedItemType);

	switch (PickedItemType)
	{
	case EItemType::EIT_Coin:
		Coins += PickedItem->GetItemInfo().ItemStatArr[0];
		return -1;
	case EItemType::EIT_HealthPotion:
		HealthPotionCount += 1;
		if (ItemMap.Contains(0) == false)
		{
			ItemMap.Add(0, PickedItem->GetItemInfo());
		}
		return 0;
	case EItemType::EIT_ManaPotion:
		ManaPotionCount += 1;
		if (ItemMap.Contains(1) == false)
		{
			ItemMap.Add(1, PickedItem->GetItemInfo());
		}
		return 1;
	case EItemType::EIT_Armour:
	case EItemType::EIT_Accessories:
		ItemMap.Add(CurrentItemUniqueNum, PickedItem->GetItemInfo());
		break;
	}
	return CurrentItemUniqueNum++;
}

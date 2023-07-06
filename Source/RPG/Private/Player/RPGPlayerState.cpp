

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

void ARPGPlayerState::UseItem(const int32& ItemNum)
{
	if (ItemNum)
	{
		ManaPotionCount -= 1;
	}
	else
	{
		HealthPotionCount -= 1;
	}
}

void ARPGPlayerState::DiscardItem(const int32& ItemNum)
{
	if (ItemNum == 0)
	{
		HealthPotionCount -= 1;
	}
	else if (ItemNum == 1)
	{
		ManaPotionCount -= 1;
	}
	else
	{
		ItemMap.Remove(ItemNum);
	}
}

FItemInfo ARPGPlayerState::GetItemInfo(const int32& ItemNum)
{
	if (ItemMap.Find(ItemNum) == nullptr)
	{
		return FItemInfo();
	}
	else
	{
		return (*ItemMap.Find(ItemNum));
	}
}

void ARPGPlayerState::AddItem(ARPGItem* PickedItem)
{
	EItemType PickedItemType = PickedItem->GetItemInfo().ItemType;
	const int32 Idx = StaticCast<int32>(PickedItemType);

	switch (PickedItemType)
	{
	case EItemType::EIT_Coin:
		Coins += PickedItem->GetItemInfo().ItemStat.CoinAmount;
		break;
	case EItemType::EIT_HealthPotion:
		HealthPotionCount += 1;
		if (ItemMap.Find(0) == nullptr)
		{
			ItemMap.Add(CurrentItemUniqueNum, PickedItem->GetItemInfo());
		}
		break;
	case EItemType::EIT_ManaPotion:
		ManaPotionCount += 1;
		if (ItemMap.Find(0) == nullptr)
		{
			ItemMap.Add(CurrentItemUniqueNum, PickedItem->GetItemInfo());
		}
		break;
	case EItemType::EIT_Armour:
	case EItemType::EIT_Weapon:
		ItemMap.Add(CurrentItemUniqueNum, PickedItem->GetItemInfo());
		break;
	}

	CurrentItemUniqueNum++;
}

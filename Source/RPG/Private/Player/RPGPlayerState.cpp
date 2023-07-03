

#include "Player/RPGPlayerState.h"
#include "../RPG.h"

void ARPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 0���� ü�� ����, 1���� ���� ����
	ItemArr.Init(FItemInfo(), 2);
}

void ARPGPlayerState::BeginPlay()
{
	Super::BeginPlay();

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
		break;
	case EItemType::EIT_ManaPotion:
		ManaPotionCount += 1;
		break;
	case EItemType::EIT_Armour:
	case EItemType::EIT_Weapon:
		ItemArr.Add(PickedItem->GetItemInfo());
		break;
	}
}

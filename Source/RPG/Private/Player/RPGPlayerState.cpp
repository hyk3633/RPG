

#include "Player/RPGPlayerState.h"
#include "Item/RPGItem.h"
#include "../RPG.h"

void ARPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ItemArray.Init(0, StaticCast<int32>(EItemType::EIT_MAX));
}

void ARPGPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGPlayerState::AddItem(ARPGItem* PickedItem)
{
	//const int32 Index = StaticCast<int32>(Type);

	//ItemArray[Index] += 1;
}

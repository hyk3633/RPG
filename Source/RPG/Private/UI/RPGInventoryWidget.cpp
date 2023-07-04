
#include "UI/RPGInventoryWidget.h"
#include "UI/RPGInventorySlotWidget.h"
#include "Item/RPGItem.h"
#include "../RPG.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void URPGInventoryWidget::InitInventory()
{
	const int32 MaxIdx = StaticCast<int32>(EItemType::EIT_MAX);
	
}

void URPGInventoryWidget::CreateInventorySlot(APlayerController* PController)
{
	if (PController == nullptr || ItemSlotClass == nullptr) return;

	// 생성된 슬롯의 열 개수
	const int32 SlotCount = ItemSlotArr.Num();
	const int32 Row = SlotCount == 0 ? 0 : SlotCount / 4;

	for (int32 i = 0; i < 4; i++)
	{
		URPGInventorySlotWidget* InvSlot = CreateWidget<URPGInventorySlotWidget>(PController, ItemSlotClass);
		
		ItemSlotArr.Add(InvSlot);

		if (InvUniformGridPanel)
		{
			// 그리드의 좌상단 부터 슬롯 추가
			UUniformGridSlot* GridSlot = InvUniformGridPanel->AddChildToUniformGrid(InvSlot, Row, i);
			GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		}
	}
}

void URPGInventoryWidget::AddCoins(const int32 CoinAmount)
{
	CoinText->SetText(FText::FromString(FString::FromInt(CoinAmount)));
}

void URPGInventoryWidget::AddPotion(const int32 SlotNum, const EItemType ItemType, const int32 PotionCount)
{
	if (ItemSlotMap.Find(SlotNum) == nullptr)
	{
		ItemSlotMap.Add(SlotNum, ItemSlotArr[LastItemSlotIndex]);
		ItemSlotArr[LastItemSlotIndex]->SaveItemToSlot(ItemType);
		ItemSlotArr[LastItemSlotIndex]->SetItemCountText(PotionCount);
		LastItemSlotIndex++;
	}
	else
	{
		(*ItemSlotMap.Find(SlotNum))->SetItemCountText(PotionCount);
	}
}

void URPGInventoryWidget::AddEquipment(const int32 SlotNum, const EItemType ItemType)
{
	ItemSlotMap.Add(SlotNum, ItemSlotArr[LastItemSlotIndex]);
	ItemSlotArr[LastItemSlotIndex]->SaveItemToSlot(ItemType);
	ItemSlotArr[LastItemSlotIndex]->SetItemCountText(1);
	LastItemSlotIndex++;
}

void URPGInventoryWidget::SetSlotIcon(const int32 SlotNum, UMaterial* Icon)
{
	(*ItemSlotMap.Find(SlotNum))->SetSlotIcon(Icon);
}

bool URPGInventoryWidget::SlotIsEmpty(const int32 SlotNum)
{
	return (ItemSlotMap.Find(SlotNum) == nullptr);
}

bool URPGInventoryWidget::IsInventoryFull()
{
	return LastItemSlotIndex == ItemSlotArr.Num();
}

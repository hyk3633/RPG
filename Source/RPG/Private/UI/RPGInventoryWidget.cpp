
#include "UI/RPGInventoryWidget.h"
#include "UI/RPGInventorySlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void URPGInventoryWidget::InitInventory()
{
	const int32 MaxIdx = StaticCast<int32>(EItemType::EIT_MAX);
	ItemCountArr.Init(0, MaxIdx);
}

void URPGInventoryWidget::CreateInventorySlot(APlayerController* PController)
{
	if (PController == nullptr || ItemSlotClass == nullptr) return;

	// 생성된 슬롯의 열 개수
	const int32 SlotCount = ItemSlotArr.Num();
	const int32 Row = SlotCount == 0 ? SlotCount : SlotCount / 4;

	for (int32 i = 0; i < 4; i++)
	{
		URPGInventorySlotWidget* InvSlot = CreateWidget<URPGInventorySlotWidget>(PController, ItemSlotClass);

		ItemSlotArr.Add(InvSlot);

		if (InvUniformGridPanel)
		{
			// 그리드의 좌상단 부터 슬롯 추가
			UUniformGridSlot* GridSlot = InvUniformGridPanel->AddChildToUniformGrid(InvSlot, Row, i);
			GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
			GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		}
	}
}

void URPGInventoryWidget::AddItem(const EItemType Type)
{
	const int32 Idx = StaticCast<int32>(Type);

	ItemCountArr[Idx]++;

	if (Type == EItemType::EIT_Coin)
	{
		CoinText->SetText(FText::FromString(FString::FromInt(ItemCountArr[Idx])));
	}
	else
	{
		if (ItemSlotMap.Find(Idx) == nullptr)
		{
			ItemSlotMap.Add(Idx, ItemSlotArr[LastItemSlotIndex]);
			ItemSlotArr[LastItemSlotIndex]->SaveItemToSlot(Type);
			ItemSlotArr[LastItemSlotIndex]->SetItemCountText(ItemCountArr[Idx]);
			LastItemSlotIndex++;
		}
		else
		{
			(*ItemSlotMap.Find(Idx))->SetItemCountText(ItemCountArr[Idx]);
		}
	}
}

bool URPGInventoryWidget::IsInventoryFull()
{
	return LastItemSlotIndex == ItemSlotArr.Num();
}

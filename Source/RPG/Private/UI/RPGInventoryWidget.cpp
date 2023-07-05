
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

void URPGInventoryWidget::AddSlotToGridPanel(URPGInventorySlotWidget* NewSlot, const int32 Row, const int32 Column)
{
	if (InvUniformGridPanel)
	{
		// 그리드의 좌상단 부터 슬롯 추가
		UUniformGridSlot* GridSlot = InvUniformGridPanel->AddChildToUniformGrid(NewSlot, Row, Column);
		GridSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		GridSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
}

void URPGInventoryWidget::SortGridPanel(URPGInventorySlotWidget* SlotToBack, const int32 LastIdx)
{
	const int32 SlotCount = InvUniformGridPanel->GetChildrenCount();
	const int32 SlotIdx = InvUniformGridPanel->GetChildIndex(SlotToBack);
	int32 Row, Column, GridChildIdx = SlotIdx + 1;
	for (Row = SlotIdx / 4; Row <= LastIdx / 4; Row++)
	{
		int32 FirstCol = (Row > (SlotIdx / 4)) ? 0 : SlotIdx % 4;
		int32 LastCol = (Row < (LastIdx / 4)) ? 4 : LastIdx % 4;
		for (Column = FirstCol; Column < LastCol; Column++)
		{
			UWidget* ChildWidget = InvUniformGridPanel->GetChildAt(GridChildIdx);
			InvUniformGridPanel->AddChildToUniformGrid(ChildWidget, Row, Column);
			GridChildIdx++;
		}
	}

	InvUniformGridPanel->AddChildToUniformGrid(SlotToBack, LastIdx / 4, LastIdx % 4);
}

void URPGInventoryWidget::RemoveSlotPage()
{
	const int32 SlotCount = InvUniformGridPanel->GetChildrenCount();
	for (int32 Idx = SlotCount - 16; Idx < SlotCount; Idx++)
	{
		InvUniformGridPanel->GetChildAt(Idx)->SetVisibility(ESlateVisibility::Hidden);
		InvUniformGridPanel->RemoveChildAt(Idx);
	}
}

void URPGInventoryWidget::AddCoins(const int32 CoinAmount)
{
	CoinText->SetText(FText::FromString(FString::FromInt(CoinAmount)));
}
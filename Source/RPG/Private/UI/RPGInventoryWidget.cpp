
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
		GridArr.Add(GridSlot);
	}
}

void URPGInventoryWidget::SortGridPanel(URPGInventorySlotWidget* SlotToBack, const int32 StartIdx, const int32 LastIdx)
{
	const int32 SlotCount = InvUniformGridPanel->GetChildrenCount();
	int32 SlotIdx = StartIdx;

	UUniformGridSlot* GridToBack = GridArr[StartIdx];

	for (int32 Idx = StartIdx; Idx < LastIdx; Idx++)
	{
		GridArr[Idx] = GridArr[Idx + 1];
	}

	const int32 StartRow = GridToBack->GetRow();
	const int32 LastRow = LastIdx / 4;
	for (int32 R = StartRow; R < LastRow + 1; R++)
	{
		const int32 StartCol = (R == StartRow) ? GridToBack->GetColumn() : 0;
		const int32 LastCol = (R == LastRow) ? (LastIdx % 4) : 4;
		for (int32 C = StartCol; C < LastCol; C++)
		{
			GridArr[SlotIdx]->SetRow(R);
			GridArr[SlotIdx]->SetColumn(C);
			GridArr[SlotIdx]->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
			GridArr[SlotIdx]->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
			SlotIdx++;
		}
	}

	GridArr[SlotIdx] = GridToBack;
	
	GridToBack->SetRow(LastIdx / 4);
	GridToBack->SetColumn(LastIdx % 4);
	GridToBack->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	GridToBack->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
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
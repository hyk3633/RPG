
#include "UI/RPGInventorySlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

bool URPGInventorySlotWidget::IsSlotEmpty()
{
	return SlottedItemType == EItemType::EIT_MAX;
}

void URPGInventorySlotWidget::SaveItemToSlot(EItemType Type)
{
	SlottedItemType = Type; 
	SetItemCountText(0);
	ItemCountText->SetVisibility(ESlateVisibility::Visible);
	ItemSlotBorder->SetBrushColor(FLinearColor::Black);
	//TODO : 아이콘 설정
}

void URPGInventorySlotWidget::SetItemCountText(const int32 Count)
{
	ItemCountText->SetText(FText::FromString(FString::FromInt(Count)));
}

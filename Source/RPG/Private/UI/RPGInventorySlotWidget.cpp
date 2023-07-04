
#include "UI/RPGInventorySlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"

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

void URPGInventorySlotWidget::SetSlotIcon(UMaterial* Icon)
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.SetImageSize(FVector2D(50, 50));
	ItemSlotIcon->SetBrush(Brush);
	ItemSlotIcon->SetBrushFromMaterial(Icon);
}

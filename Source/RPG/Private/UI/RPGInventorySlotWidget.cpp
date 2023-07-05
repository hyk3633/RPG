
#include "UI/RPGInventorySlotWidget.h"
#include "../RPG.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Button.h"

void URPGInventorySlotWidget::BindButtonEvent()
{
	ItemSlotButton->OnClicked.AddDynamic(this, &URPGInventorySlotWidget::OnItemSlotButtonClicked);
	
}

void URPGInventorySlotWidget::SaveItemToSlot(EItemType Type)
{
	SlottedItemType = Type; 
	SetItemCountText(0);
	ItemCountText->SetVisibility(ESlateVisibility::Visible);
	ItemSlotBorder->SetBrushColor(FLinearColor::Black);
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

void URPGInventorySlotWidget::ClearSlot()
{
	// TODO : Image Size
	ItemSlotIcon->SetBrushFromMaterial(nullptr);
	ItemSlotBorder->SetBrushColor(FLinearColor::White);
	ItemCountText->SetVisibility(ESlateVisibility::Hidden);
}

void URPGInventorySlotWidget::OnItemSlotButtonClicked()
{
	DOnIconButtonClicked.Broadcast(UniqueNumber);
}

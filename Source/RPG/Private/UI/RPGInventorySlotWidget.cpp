
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

	ItemSlotButton->OnHovered.AddDynamic(this, &URPGInventorySlotWidget::OnItemSlotButtonHovered);
	ItemSlotButton->OnUnhovered.AddDynamic(this, &URPGInventorySlotWidget::OnItemSlotButtonUnhovered);
}

void URPGInventorySlotWidget::SetItemCountText(const int32 Count)
{
	ItemCountText->SetText(FText::FromString(FString::FromInt(Count)));
}

void URPGInventorySlotWidget::SetSlotIcon(UMaterial* Icon)
{
	IconMaterial = Icon;
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.SetImageSize(FVector2D(50, 50));
	ItemSlotIcon->SetBrush(Brush);
	ItemSlotIcon->SetBrushFromMaterial(Icon);
	ItemSlotIcon->SetVisibility(ESlateVisibility::Visible);
}

void URPGInventorySlotWidget::ClearSlot()
{
	// TODO : Image Size
	ItemSlotIcon->SetBrushFromMaterial(nullptr);
	ItemSlotIcon->SetVisibility(ESlateVisibility::Hidden);
	ItemSlotBorder->SetBrushColor(FLinearColor(0.2f, 0.2f, 0.2f));
	ItemCountText->SetVisibility(ESlateVisibility::Hidden);
	UniqueNumber = -1;

	ItemSlotButton->OnHovered.RemoveAll(this);
	ItemSlotButton->OnUnhovered.RemoveAll(this);
}

void URPGInventorySlotWidget::SetBorderStateToEquipped(const bool bIsEquipped)
{
	if (bIsEquipped)
	{
		ItemSlotBorder->SetBrushColor(FLinearColor(1, 0.5f, 0));
	}
	else
	{
		ItemSlotBorder->SetBrushColor(FLinearColor::Black);
	}
}

void URPGInventorySlotWidget::OnItemSlotButtonClicked()
{
	DOnIconButtonClicked.Broadcast(UniqueNumber);
}

void URPGInventorySlotWidget::OnItemSlotButtonHovered()
{
	if (UniqueNumber != -1 && !bIsHovered)
	{
		DOnIconButtonHovered.Broadcast(UniqueNumber);
		bIsHovered = true;
	}
}

void URPGInventorySlotWidget::OnItemSlotButtonUnhovered()
{
	if (UniqueNumber != -1 && bIsHovered)
	{
		DOnIconButtonHovered.Broadcast(-1);
		bIsHovered = false;
	}
}

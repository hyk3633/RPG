
#include "UI/RPGItemSlotMenuWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/ScaleBox.h"
#include "Components/TextBlock.h"

void URPGItemSlotMenuWidget::SetWidgetPosition(const FVector2D& Position)
{
	UCanvasPanelSlot* GridAsPanelSlot = Cast<UCanvasPanelSlot>(WidgetBox->Slot);
	GridAsPanelSlot->SetPosition(Position);
}

void URPGItemSlotMenuWidget::SetUseText(FString ButtonText)
{
	UseText->SetText(FText::FromString(ButtonText));
}

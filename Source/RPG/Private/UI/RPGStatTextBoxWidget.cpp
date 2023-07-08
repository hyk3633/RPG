
#include "UI/RPGStatTextBoxWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"

void URPGStatTextBoxWidget::SetStatText(const FString& StatString)
{
	StatInfoText->SetText(FText::FromString(StatString));
}

void URPGStatTextBoxWidget::SetWidgetPosition(const FVector2D& Position)
{
	UCanvasPanelSlot* GridAsPanelSlot = Cast<UCanvasPanelSlot>(StatOverlayBox->Slot);
	GridAsPanelSlot->SetPosition(Position);
}
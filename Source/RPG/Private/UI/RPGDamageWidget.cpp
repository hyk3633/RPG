
#include "UI/RPGDamageWidget.h"
#include "Components/TextBlock.h"

void URPGDamageWidget::InitDamageWidget()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void URPGDamageWidget::SetDamageTextAndPopup(const int32 Damage)
{
	bActivated = true;
	DamageText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Damage)));
	SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(PopUp);
}

void URPGDamageWidget::DeactivateWidget()
{
	bActivated = false;
	SetVisibility(ESlateVisibility::Hidden);
}

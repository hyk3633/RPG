
#include "UI/RPGStatInfoWidget.h"
#include "Player/RPGPlayerState.h"
#include "Components/TextBlock.h"

void URPGStatInfoWidget::UpdateStatCharacterStatText(const FStatInfo& Stats)
{
	UpdateTextBlock(DefCharacterText, Stats.DefenseivePower);
	UpdateTextBlock(DexCharacterText, Stats.Dexterity);
	UpdateTextBlock(MxHPCharacterText, Stats.MaxHP);
	UpdateTextBlock(MxMPCharacterText, Stats.MaxMP);
	UpdateTextBlock(StkCharacterText, Stats.StrikingPower);
	UpdateTextBlock(SkpCharacterText, Stats.SkillPower);
	UpdateTextBlock(AtksCharacterText, Stats.AttackSpeed);
}

void URPGStatInfoWidget::UpdateStatEquippedItemStatText(const FStatInfo& Stats)
{
	UpdateTextBlock(DefItemText, Stats.DefenseivePower, true);
	UpdateTextBlock(DexItemText, Stats.Dexterity, true);
	UpdateTextBlock(MxHPItemText, Stats.MaxHP, true);
	UpdateTextBlock(MxMPItemText, Stats.MaxMP, true);
	UpdateTextBlock(StkItemText, Stats.StrikingPower, true);
	UpdateTextBlock(SkpItemText, Stats.SkillPower, true);
	UpdateTextBlock(AtksItemText, Stats.AttackSpeed, true);
}

void URPGStatInfoWidget::UpdateTextBlock(UTextBlock* TextBlock, const float& Num, const bool bIsItemStat)
{
	if (bIsItemStat)
	{
		if (Num == 0) TextBlock->SetVisibility(ESlateVisibility::Hidden);
		else TextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("(+%.1f)"), Num)));
	}
	else
	{
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), Num)));
	}
}

void URPGStatInfoWidget::UpdateTextBlock(UTextBlock* TextBlock, const int32& Num, const bool bIsItemStat)
{
	if (bIsItemStat)
	{
		if (Num == 0) TextBlock->SetVisibility(ESlateVisibility::Hidden);
		else TextBlock->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("(+%d)"), Num)));
	}
	else
	{
		TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d"), Num)));
	}
}

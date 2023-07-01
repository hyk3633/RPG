
#include "UI/RPGItemNameTag.h"
#include "Components/TextBlock.h"

void URPGItemNameTag::SetNameTagText(FString StrName)
{
	NameTag->SetText(FText::FromString(StrName));
}

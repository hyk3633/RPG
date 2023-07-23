
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/StatInfo.h"
#include "RPGStatInfoWidget.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class RPG_API URPGStatInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void UpdateStatCharacterStatText(const FStatInfo& Stats);

	void UpdateStatEquippedItemStatText(const FStatInfo& Stats);

protected:

	void UpdateTextBlock(UTextBlock* TextBlock, const float& Num, const bool bIsItemStat = false);

	void UpdateTextBlock(UTextBlock* TextBlock, const int32& Num, const bool bIsItemStat = false);

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StkCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StkItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkpCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkpItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DexCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DexItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AtksCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AtksItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MxHPCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MxHPItemText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MxMPCharacterText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MxMPItemText;

};

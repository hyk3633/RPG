
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/ItemType.h"
#include "RPGInventorySlotWidget.generated.h"

/**
 * 
 */

class UButton;
class UBorder;
class UTextBlock;

UCLASS()
class RPG_API URPGInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	bool IsSlotEmpty();

	void SaveItemToSlot(EItemType Type);

	void SetItemCountText(const int32 Count);

private:

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemSlotBorder;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemSlotIconButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	EItemType SlottedItemType;

};


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
class UImage;

UCLASS()
class RPG_API URPGInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	bool IsSlotEmpty();

	void SaveItemToSlot(EItemType Type);

	void SetItemCountText(const int32 Count);

	void SetSlotIcon(UMaterial* Icon);

	FORCEINLINE void SetUniqueNumber(int32 Num) { UniqueNumber = Num; }

private:

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemSlotBorder;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemSlotButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemCountText;

	EItemType SlottedItemType;

	int32 UniqueNumber;

};

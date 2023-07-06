
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/ItemType.h"
#include "RPGInventorySlotWidget.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIconSlotButtonClickedDelegate, int32 SlotUniqueNumber);

class UButton;
class UBorder;
class UTextBlock;
class UImage;

UCLASS()
class RPG_API URPGInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindButtonEvent();

	void SaveItemToSlot(EItemType Type);

	void SetItemCountText(const int32 Count);

	void SetSlotIcon(UMaterial* Icon);

	void ClearSlot();

	FORCEINLINE bool IsSlotEmpty() { return SlottedItemType == EItemType::EIT_MAX; };
	FORCEINLINE void SetUniqueNumber(int32 Num) { UniqueNumber = Num; }
	FORCEINLINE UButton* GetItemSlotButton() const { return ItemSlotButton; }
	FORCEINLINE void SetSlotRowColumn(int32 Row, int32 Col) { SlotRow = Row, SlotColumn = Col; }
	FORCEINLINE int32 GetSlotRow() const { return SlotRow; }
	FORCEINLINE int32 GetSlotColumn() const { return SlotColumn; }

	FOnIconSlotButtonClickedDelegate DOnIconButtonClicked;

protected:

	UFUNCTION()
	void OnItemSlotButtonClicked();

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

	int32 SlotRow;

	int32 SlotColumn;

};

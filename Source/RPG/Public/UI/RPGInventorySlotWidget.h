
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/ItemType.h"
#include "RPGInventorySlotWidget.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIconSlotButtonClickedDelegate, int32 SlotUniqueNumber);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnIconSlotButtonHoveredDelegate, int32 SlotUniqueNumber);

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

	void SetBorderStateToEquipped(const bool bIsEquipped);

	FORCEINLINE EItemType GetSavedItemType() const { return SlottedItemType; };
	FORCEINLINE void SetUniqueNumber(int32 Num) { UniqueNumber = Num; }
	FORCEINLINE UMaterial* GetIconMaterial() const { return IconMaterial; }

	FOnIconSlotButtonClickedDelegate DOnIconButtonClicked;

	FOnIconSlotButtonHoveredDelegate DOnIconButtonHovered;

protected:

	UFUNCTION()
	void OnItemSlotButtonClicked();

	UFUNCTION()
	void OnItemSlotButtonHovered();

	UFUNCTION()
	void OnItemSlotButtonUnhovered();

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

	UMaterial* IconMaterial;

	bool bIsHovered = false;
};

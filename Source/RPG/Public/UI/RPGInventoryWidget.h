
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/ItemType.h"
#include "RPGInventoryWidget.generated.h"

/**
 * 
 */

class UUniformGridPanel;
class UUniformGridSlot;
class UTextBlock;
class URPGInventorySlotWidget;
class ARPGItem;
class UImage;
class UButton;

UCLASS()
class RPG_API URPGInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void InitInventory();

	void AddSlotToGridPanel(URPGInventorySlotWidget* NewSlot, const int32 Row, const int32 Column);

	void SortGridPanel(URPGInventorySlotWidget* SlotToBack, const int32 StartIdx, const int32 LastIdx);

	void RemoveSlotPage();

	void AddCoins(const int32 CoinAmount);

	void SetEquipmentSlot(const EItemType ItemType, UMaterial* IconMat);

	void ClearEquipmentSlot(const EItemType ItemType);

private:

	UPROPERTY(meta = (BindWidget))
	UImage* ArmourSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* ArmourSlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* AccessoriesSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UButton* AccessoriesSlotButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinText;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InvUniformGridPanel;

	UPROPERTY()
	TArray<UUniformGridSlot*> GridArr;

};


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enums/ItemType.h"
#include "RPGInventoryWidget.generated.h"

/**
 * 
 */

class UUniformGridPanel;
class UTextBlock;
class URPGInventorySlotWidget;

UCLASS()
class RPG_API URPGInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void InitInventory();

	void CreateInventorySlot(APlayerController* PController);

	void AddItem(const EItemType Type);

	bool IsInventoryFull();

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CoinText;

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InvUniformGridPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<URPGInventorySlotWidget> ItemSlotClass;

	UPROPERTY()
	TArray<URPGInventorySlotWidget*> ItemSlotArr;

	// 아이템의 종류 별 갯수
	TArray<int32> ItemCountArr;

	// Key : 아이템 Enum, Value : 그 아이템을 저장하고 있는 슬롯 포인터
	TMap<int32, URPGInventorySlotWidget*> ItemSlotMap;
	
	// 아이템이 저장되어 있는 가장 마지막 슬롯 인덱스
	int32 LastItemSlotIndex = 0;

};

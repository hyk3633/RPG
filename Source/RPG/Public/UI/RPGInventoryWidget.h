
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

	// �������� ���� �� ����
	TArray<int32> ItemCountArr;

	// Key : ������ Enum, Value : �� �������� �����ϰ� �ִ� ���� ������
	TMap<int32, URPGInventorySlotWidget*> ItemSlotMap;
	
	// �������� ����Ǿ� �ִ� ���� ������ ���� �ε���
	int32 LastItemSlotIndex = 0;

};

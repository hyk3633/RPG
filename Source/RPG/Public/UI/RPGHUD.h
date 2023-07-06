
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Enums/ItemType.h"
#include "RPGHUD.generated.h"

/**
 * 
 */

class URPGGameplayInterface;
class ARPGBasePlayerCharacter;
class URPGInventoryWidget;
class URPGInventorySlotWidget;
class ARPGItem;
class UDataTable;
class URPGItemSlotMenuWidget;

UCLASS()
class RPG_API ARPGHUD : public AHUD
{
	GENERATED_BODY()

public:

	ARPGHUD();

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;
	
protected:

	virtual void BeginPlay() override;

public:

	void InitHUD();

protected:

	void OffHUD();

	void DrawOverlay();

	/** ü��, ���� ���α׷��� �� */

	UFUNCTION()
	void SetHealthBarPercentage(float Percentage);

	UFUNCTION()
	void SetManaBarPercentage(float Percentage);

	/** ��ų ��ٿ� */

	void UpdateCooldownProgress();

	void SetProgressPercentage(const int8 Index, const float Percentage);

	UFUNCTION()
	void CooldownProgressSetFull(uint8 Bit);

	/** �κ��丮 */

	void InitInventorySlot();

public: 

	void AddCoins(const int32& CoinAmount);

	void AddPotion(const int32& UniqueNum, const EItemType ItemType, const int32& PotionCount);

	void AddEquipment(const int32& UniqueNum, const EItemType ItemType);

	void UpdateItemCount(const int32& UniqueNum, const int32& ItemCount);

protected:

	void ClearItemSlot(const int32& UniqueNum);

	void SetSlotIcon(const int32& UniqueNum, const EItemType ItemType);

	void ExpandInventoryIfNoSpace();

	UFUNCTION()
	void OnItemSlotButtonClickEvent(int32 UniqueNum);

	UFUNCTION()
	void OnUseButtonClicked();

	UFUNCTION()
	void OnDiscardButtonClicked();

private:

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> GameplayInterfaceClass;

	UPROPERTY()
	URPGGameplayInterface* GameplayInterface;

	UPROPERTY()
	ARPGBasePlayerCharacter* PlayerPawn;

	UPROPERTY()
	UMaterialInstance* ClockProgressMatInst;

	UPROPERTY()
	UMaterialInstanceDynamic* ClockProgressMatInstDynamic;

	FTimerHandle OffTimer;

	// �κ��丮

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<URPGInventorySlotWidget> ItemSlotClass;

	UPROPERTY()
	TArray<URPGInventorySlotWidget*> ActivadtedSlots;

	UPROPERTY()
	TArray<URPGInventorySlotWidget*> EmptySlots;

	UPROPERTY()
	TMap<int32, URPGInventorySlotWidget*> ItemSlotMap;

	// �������� ����� ���� ����
	int32 SavedItemSlotCount = 0;

	// Ȱ��ȭ�� ���� ����
	int32 ActivatedItemSlotNum = 0;

	UPROPERTY(EditAnywhere, Category = "HUD")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<URPGItemSlotMenuWidget> ItemSlotMenuClass;

	UPROPERTY()
	URPGItemSlotMenuWidget* ItemSlotMenuWidget;

	bool bIsItemSlotMenuWidgetOn = false;

	int32 SelectedItemUniqueNum;
};
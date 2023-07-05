
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

USTRUCT(Atomic)
struct FItemSlotStruct
{
	GENERATED_BODY()
public:

	UPROPERTY()
	URPGInventorySlotWidget* ItemSlot;

	UPROPERTY()
	int32 UniqueNum;

	FItemSlotStruct() : ItemSlot(nullptr), UniqueNum(-1) {}

	FItemSlotStruct(URPGInventorySlotWidget* NewSlot) : ItemSlot(NewSlot), UniqueNum(-1) {}

	FItemSlotStruct(URPGInventorySlotWidget* NewSlot, int32 NewNum) : ItemSlot(NewSlot), UniqueNum(NewNum) {}

};

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

	/** 체력, 마나 프로그레스 바 */

	UFUNCTION()
	void SetHealthBarPercentage(float Percentage);

	UFUNCTION()
	void SetManaBarPercentage(float Percentage);

	/** 스킬 쿨다운 */

	void UpdateCooldownProgress();

	void SetProgressPercentage(const int8 Index, const float Percentage);

	UFUNCTION()
	void CooldownProgressSetFull(uint8 Bit);

	/** 인벤토리 */

	void InitInventorySlot();

public: 

	void AddCoins(const int32 CoinAmount);

	void AddPotion(const int32 UniqueNum, const EItemType ItemType, const int32 PotionCount);

	int32 GetEmptySlotIndex();

	void AddEquipment(const int32 UniqueNum, const EItemType ItemType);

	void UpdatePotionCount(const int32 UniqueNum, const EItemType ItemType, const int32 PotionCount);

protected:

	void ClearItemSlot(const int32 UniqueNum);

	void SetSlotIcon(const int32 UniqueNum, const EItemType ItemType);

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

	// 인벤토리

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<URPGInventorySlotWidget> ItemSlotClass;

	UPROPERTY()
	TArray<FItemSlotStruct> ItemSlotArr;

	UPROPERTY()
	TMap<int32, URPGInventorySlotWidget*> ItemSlotMap;

	TArray<int32> EmptySlotIndexArr;

	int32 LastStoredSlotNum = 0;

	int32 ActivatedItemSlotNum = 0;

	int32 StoredSlotCount = 0;

	UPROPERTY(EditAnywhere, Category = "HUD")
	UDataTable* ItemDataTable;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<URPGItemSlotMenuWidget> ItemSlotMenuClass;

	UPROPERTY()
	URPGItemSlotMenuWidget* ItemSlotMenuWidget;

	bool bIsItemSlotMenuWidgetOn = false;

	int32 SelectedItemUniqueNum;
};
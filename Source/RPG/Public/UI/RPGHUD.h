
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
class ARPGItem;
class UDataTable;

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

private:

	void OffHUD();

private:

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

public: /** �κ��丮 */

	void AddCoins(const int32 CoinAmount);

	void AddPotion(const int32 SlotNum, const EItemType ItemType, const int32 PotionCount);

	void AddEquipment(const int32 SlotNum, const EItemType ItemType);

protected:

	void SetSlotIcon(const int32 SlotNum, const EItemType ItemType);

	void ExpandInventoryIfNoSpace();

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

	UPROPERTY(EditAnywhere, Category = "HUD")
	UDataTable* ItemDataTable;
};
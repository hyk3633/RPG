
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"

/**
 * 
 */

class ARPGBasePlayerCharacter;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ARPGItem;

UCLASS()
class RPG_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ARPGPlayerController();

	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

	UFUNCTION()
	void OnRep_MyCharacter();

public:

	virtual void Tick(float DeltaTime) override;

protected:

	void ItemTrace();

	virtual void SetupInputComponent() override;

	void LeftClickAction_StopMove();

	void LeftClickAction_SetPath();

	UFUNCTION(Server, Reliable)
	void PickupItemServer(ARPGItem* Item);

	void PickupItem(ARPGItem* Item);

	UFUNCTION(Client, Reliable)
	void PickupCoinsClient(const int32 CoinAmount);

	UFUNCTION(Client, Reliable)
	void PickupPotionClient(const int32 UniqueNum, const EItemType PotionType, const int32 PotionCount);

	UFUNCTION(Client, Reliable)
	void PickupEquipmentClient(const int32 UniqueNum, const EItemType ItemType);

public:

	void UseItem(const int32& UniqueNum);

protected:

	UFUNCTION(Server, Reliable)
	void UseItemServer(const int32 UniqueNum);

	int32 GetItemCount(const int32 UniqueNum);

	UFUNCTION(Client, Reliable)
	void UpdateItemInfoClient(const int32 UniqueNum, const int32 ItemCount);

public:

	void EquipOrUnequipItem(const int32& UniqueNum);

protected:

	UFUNCTION(Server, Reliable)
	void EquipOrUnequipItemServer(const int32 UniqueNum);

public:

	void DiscardItem(const int32& UniqueNum);

protected:

	UFUNCTION(Server, Reliable)
	void DiscardItemServer(const int32 UniqueNum);

public:

	void GetStatInfoText(const int32& UniqueNum);

protected:

	UFUNCTION(Server, Reliable)
	void GetStatInfoTextServer(const int32 UniqueNum);

	UFUNCTION(Client, Reliable)
	void GetStatInfoTextClient(const FString& StatString);

protected:

	void RightClick_AttackOrSetAbilityPoint();

	void QPressedAction_Cast();

	void WPressedAction_Cast();

	void EPressedAction_Cast();

	void RPressedAction_Cast();

	void MouseWheelScroll_ZoomInOut(const FInputActionValue& Value);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY(ReplicatedUsing = OnRep_MyCharacter)
	ARPGBasePlayerCharacter* MyCharacter;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClickAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightClickAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QPressedAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* WPressedAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EPressedAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RPressedAction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MouseWheelScroll;

	UPROPERTY()
	ARPGItem* TracedItem;
};


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

	virtual void SetupInputComponent() override;

	void LeftClickAction_StopMove();

	void LeftClickAction_SetPath();

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
};

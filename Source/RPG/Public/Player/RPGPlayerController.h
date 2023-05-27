
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

public:

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void SetupInputComponent() override;

	void SetDestinationClick_StopMove();

	void SetDestinationClick_SetPath();

	void NormalAttackClick_NormalAttack();

	void Ability_Q_PressedAction_Cast();

	void Ability_W_PressedAction_Cast();

	void Ability_E_PressedAction_Cast();

	void Ability_R_PressedAction_Cast();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY(Replicated)
	ARPGBasePlayerCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* NormalAttackClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Ability_Q_PressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Ability_W_PressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Ability_E_PressedAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Ability_R_PressedAction;
};

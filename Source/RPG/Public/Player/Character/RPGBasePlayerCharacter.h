
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBasePlayerCharacter.generated.h"

class ARPGPlayerController;
class URPGAnimInstance;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RPG_API ARPGBasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBasePlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:	

	void DoNormalAttack(const FVector& AttackPoint);

	void NormalAttackWithCombo(const FVector& AttackPoint);

protected:

	UFUNCTION(Server, Reliable)
	void PlayAttackEffectServer(const FVector& AttackPoint);

	UFUNCTION(NetMulticast, Reliable)
	void PlayAttackEffectMulticast(const FVector& AttackPoint);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void TurnTowardAttackPoint(const FVector& AttackPoint);

	void PlayerDie();

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	void AttackStartComboState();
	void AttackEndComboState();

	UFUNCTION()
	void NormalAttackNextCombo();

private:

	ARPGPlayerController* RPGPlayerController;

	URPGAnimInstance* RPGAnimInstance;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	bool bIsAttacking = false;

	bool CanNextCombo = false;

	bool IsComboInputOn;

	int32 CurrentCombo = 0;

	int32 MaxCombo = 4;
};

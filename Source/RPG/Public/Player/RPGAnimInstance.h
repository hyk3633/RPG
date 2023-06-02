
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/PressedKey.h"
#include "RPGAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackInputCheckDelegate);
DECLARE_DELEGATE(FOnAbility_Cast);

UCLASS()
class RPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayNormalAttackMontage();

	void JumpToAttackMontageSection(int32 NewSection);

	void PlayAbilityMontageOfKey(bool bJumpToSection = false);

	void PlayAbilityMontage(UAnimMontage* AbilityMontage, bool bJumpToSection);

	void PlayDeathMontage();

	FORCEINLINE void SetCurrentState(EPressedKey KeyType) { CurrentKeyState = KeyType; }
	FORCEINLINE EPressedKey GetCurrentState() const { return CurrentKeyState; }
	FORCEINLINE void AimingPoseOn() { bAimingPose = true; };
	FORCEINLINE void AimingPoseOff() { bAimingPose = false; };
	FORCEINLINE void SetMaxCombo(const int8 MaxValue) { MaxCombo = MaxValue; }

	FOnAttackInputCheckDelegate DOnAttackInputCheck;

	FOnAbility_Cast DOnAbility_Q_Cast;
	FOnAbility_Cast DOnAbility_W_Cast;
	FOnAbility_Cast DOnAbility_E_Cast;
	FOnAbility_Cast DOnAbility_R_Cast;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_AttackInputCheck();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Ability_Q_Cast();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Ability_W_Cast();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Ability_E_Cast();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Ability_R_Cast();

private:

	FName GetAttackMontageSectionName(int32 Section);

private:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* NormalAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_Q_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_W_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_E_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_R_Montage;

	UPROPERTY(BlueprintReadOnly, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	bool bAimingPose = false;

	UPROPERTY(BlueprintReadOnly, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	EPressedKey CurrentKeyState = EPressedKey::EPK_None;

	int8 MaxCombo;
};

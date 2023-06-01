
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

	void PlayAbilityMontage(EPressedKey KeyType);

	void PlayDeathMontage();

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
};

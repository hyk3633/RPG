
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackInputCheckDelegate);
DECLARE_DELEGATE(FOnAbility_Q_Cast);

UCLASS()
class RPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayNormalAttackMontage();

	void JumpToAttackMontageSection(int32 NewSection);

	void PlayAbility_Q_Montage();
	void PlayAbility_W_Montage();
	void PlayAbility_E_Montage();
	void PlayAbility_R_Montage();

	void PlayDeathMontage();

	FOnAttackInputCheckDelegate DOnAttackInputCheck;

	FOnAbility_Q_Cast DOnAbility_Q_Cast;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_AttackInputCheck();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Ability_Q_Cast();

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

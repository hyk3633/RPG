
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackInputCheckDelegate);

UCLASS()
class RPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayDeathMontage();

	void PlayNormalAttackMontage();

	void JumpToAttackMontageSection(int32 NewSection);

	FOnAttackInputCheckDelegate DOnAttackInputCheck;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_AttackInputCheck();

private:

	FName GetAttackMontageSectionName(int32 Section);

private:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* NormalAttackMontage;

};


#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate);

/**
 * 
 */
UCLASS()
class RPG_API URPGEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayAttackMontage();

	void PlayDeathMontage();

	FOnAttackDelegate DOnAttack;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Attack();

private:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

public:

	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }

};

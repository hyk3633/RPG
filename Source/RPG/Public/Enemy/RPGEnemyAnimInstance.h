
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate);
DECLARE_MULTICAST_DELEGATE(FOnRangedAttackDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndedDelegate);

/**
 * 
 */

class ARPGBaseEnemyCharacter;

UCLASS()
class RPG_API URPGEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void BindFunction();

	void PlayMeleeAttackMontage();

	void PlayRangedAttackMontage();

	void PlayDeathMontage();

	void PlayGetupMontage();

	void CancelMontage();

	FOnAttackDelegate DOnAttack;

	FOnRangedAttackDelegate DOnRangedAttack;

	FOnAttackEndedDelegate DOnAttackEnded;

	friend ARPGBaseEnemyCharacter;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Attack();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_RangedAttack();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	UPROPERTY()
	ARPGBaseEnemyCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* MeleeAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* RangedAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* GetupMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

};

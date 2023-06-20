
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate);

/**
 * 
 */

class ARPGBaseEnemyCharacter;

UCLASS()
class RPG_API URPGEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayAttackMontage();

	void PlayDeathMontage();

	void PlayGetupMontage();

	FOnAttackDelegate DOnAttack;

	friend ARPGBaseEnemyCharacter;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Attack();

private:

	UPROPERTY()
	ARPGBaseEnemyCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

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

public:

	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }

};

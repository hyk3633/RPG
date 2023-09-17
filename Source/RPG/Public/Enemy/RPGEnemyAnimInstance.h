
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackDelegate);
DECLARE_MULTICAST_DELEGATE(FOnRangedAttackDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndedDelegate);
DECLARE_MULTICAST_DELEGATE(FHitReactionEndedDelegate);
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

	void PlayMeleeAttackMontage(const bool HasAuthority);

	void PlayRangedAttackMontage(const bool HasAuthority);

	void PlayDeathMontage();

	void PlayGetupMontage();

	void PlayHitReactionMontage();

	void CancelMontage();

	FOnAttackDelegate DOnAttack;

	FOnRangedAttackDelegate DOnRangedAttack;

	FOnAttackEndedDelegate DOnAttackEnded;

	FHitReactionEndedDelegate DOnHitReactionEnded;

	friend ARPGBaseEnemyCharacter;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_Attack();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_RangedAttack();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	UPROPERTY()
	ARPGBaseEnemyCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ClientMeleeAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ServerMeleeAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ClientRangedAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ServerRangedAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* GetupMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Status", meta = (AllowPrivateAccess = "true"))
	bool bIsDead;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RPGEnemyAnimInstance.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackTraceDelegate);

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

	FOnAttackTraceDelegate DOnAttackTrace;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_AttackTrace();

private:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

public:

	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }

};

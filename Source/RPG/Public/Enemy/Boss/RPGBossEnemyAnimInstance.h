
#pragma once

#include "CoreMinimal.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enums/SpecialAttackType.h"
#include "RPGBossEnemyAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpecialAttackDelegate, ESpecialAttackType Type);
DECLARE_MULTICAST_DELEGATE(FOnSpecialAttackEndDelegate);

UCLASS()
class RPG_API URPGBossEnemyAnimInstance : public URPGEnemyAnimInstance
{
	GENERATED_BODY()

public:

	virtual void BindFunction() override;

	void PlayEmitShockWaveMontage();

	void PlayFireMortarMontage();

	void PlayBulldozeMontage();

	bool GetIsTurning() const;

	FORCEINLINE void SetStunned(const bool bIsStunned) { bStunned = bIsStunned; }
	FORCEINLINE void SetPitch(const float& Pitch) { AimPitch = Pitch; }
	FORCEINLINE void SetYaw(const float& Yaw) { AimYaw = Yaw; }

	FOnSpecialAttackDelegate DOnSpecialAttack;
	FOnSpecialAttackEndDelegate DOnSpecialAttackEnd;

protected:

	UFUNCTION(BlueprintCallable)
	void AnimNotify_SpecialAttack(ESpecialAttackType Type);

	UFUNCTION()
	void OnSpecialAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
protected:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* EmitShockWaveMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* FireMortarMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* BulldozeMontage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bStunned = false;
};

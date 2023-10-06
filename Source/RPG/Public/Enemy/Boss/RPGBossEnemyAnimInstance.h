
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

	void BindBossEnemyFunction();

	void PlayEmitShockWaveMontage(const bool HasAuthority);

	void PlayFireMortarMontage(const bool HasAuthority);

	void PlayBulldozeMontage(const bool HasAuthority);

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
	UAnimMontage* ClientEmitShockWaveMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ServerEmitShockWaveMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ClientFireMortarMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ServerFireMortarMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ClientBulldozeMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ServerBulldozeMontage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimPitch;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimYaw;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bStunned;
};

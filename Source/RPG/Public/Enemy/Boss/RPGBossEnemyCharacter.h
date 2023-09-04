
#pragma once

#include "CoreMinimal.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enums/SpecialAttackType.h"
#include "RPGBossEnemyCharacter.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnSpecialAttackMontageEndDelegate);

class URPGBossEnemyAnimInstance;

UCLASS()
class RPG_API ARPGBossEnemyCharacter : public ARPGBaseEnemyCharacter
{
	GENERATED_BODY()
	
public:

	ARPGBossEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void ActivateEnemy(const FVector& Location) override;

protected:

	void CalculateAimYawAndPitch(float DeltaTime);

	void StunCleared();

	UFUNCTION()
	void OnRep_AimPitch();

	UFUNCTION()
	void OnRep_AimYaw();

	UFUNCTION(NetMulticast, Reliable)
	void SetAimYawMulticast(const float& NewYaw);

public:

	virtual void BTTask_Attack() override;

	FOnSpecialAttackMontageEndDelegate DOnSpecialAttackEnd;

protected:

	virtual void HealthDecrease(const int32& Damage) override;

	virtual void InitAnimInstance() override;

	virtual void Attack() override;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnParticleMulticast(UParticleSystem* Particle, const FVector_NetQuantize& Location, const FRotator& Rotation);

public:

	void BTTask_SpecialAttack();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void PlayEmitShockwaveMontageEffectMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void PlayFireMortarMontageEffectMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void PlayBulldozeMontageEffectMulticast();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateAttackRangeMarkMulticast(ESpecialAttackType Type, const float Size);

	UFUNCTION()
	void EmitShockWave(ESpecialAttackType Type);

	UFUNCTION()
	void FireMortar(ESpecialAttackType Type);

	UFUNCTION()
	void Bulldoze(ESpecialAttackType Type);

	virtual void OnAttackMontageEnded() override;

	UFUNCTION()
	void OnSpecialAttackMontageEnded();

	void SpecialAttackCooldownEnd();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY()
	URPGBossEnemyAnimInstance* BossAnimInst;

	UPROPERTY(VisibleInstanceOnly)
	UStaticMeshComponent* AttackRangeMark;

	UPROPERTY()
	UMaterial* Circle;

	UPROPERTY()
	UMaterial* Rectangle;

	UPROPERTY(Replicated)
	FVector TargetLocation;

	UPROPERTY(ReplicatedUsing = OnRep_AimPitch)
	float AimPitch;

	UPROPERTY(ReplicatedUsing = OnRep_AimYaw)
	float AimYaw;

	float TargetAimYaw;

	float LastframeInitYaw;

	float InitYaw = 0.f;

	float StiffTimeLimit = 0.f;

	int32 CumulativeDamage = 0;

	bool bIsStunned = false;

	FTimerHandle StunTimer;

	/** 기본 공격 */

	UPROPERTY()
	UParticleSystem* PrimaryAttackParticle;

	UPROPERTY()
	UParticleSystem* PrimaryAttackWorldImpactParticle;

	UPROPERTY()
	UParticleSystem* PrimaryAttackCharacterImpactParticle;

	FTimerHandle AttackDelayTimer;

	/** 구형 범위 공격 */
	UPROPERTY()
	UParticleSystem* RangeAttackImpactParticle;

	FTimerHandle SpecialAttackTimer;

	bool bIsAttacking = false;

	FTimerHandle SpecialAttackCooldownTimer;
};

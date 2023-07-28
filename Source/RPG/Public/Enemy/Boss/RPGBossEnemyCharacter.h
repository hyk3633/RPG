
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

public:

	virtual void Tick(float DeltaTime) override;

	virtual void BTTask_Attack() override;

	FOnSpecialAttackMontageEndDelegate DOnSpecialAttackEnd;

protected:

	void CalculateAimYawAndPitch(float DeltaTime);

	UFUNCTION()
	void OnRep_AimPitch();

	UFUNCTION()
	void OnRep_AimYaw();

	UFUNCTION(NetMulticast, Reliable)
	void SetAimYawMulticast(const float& NewYaw);

	virtual void InitAnimInstance() override;

	virtual void PlayMeleeAttackEffect() override;

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

	UFUNCTION()
	void OnSpecialAttackMontageEnded();

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

	/** 기본 공격 */

	UPROPERTY()
	UParticleSystem* PrimaryAttackParticle;

	UPROPERTY()
	UParticleSystem* PrimaryAttackWorldImpactParticle;

	UPROPERTY()
	UParticleSystem* PrimaryAttackCharacterImpactParticle;

	/** 구형 범위 공격 */
	UPROPERTY()
	UParticleSystem* RangeAttackImpactParticle;

	FTimerHandle SpecialAttackTimer;

};

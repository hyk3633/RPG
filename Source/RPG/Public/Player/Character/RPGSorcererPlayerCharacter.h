
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enums/NotifyCode.h"
#include "RPGSorcererPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGBaseProjectile;
class ARPGRestrictionProjectile;
class ARPGBlackhole;

UCLASS()
class RPG_API ARPGSorcererPlayerCharacter : public ARPGBasePlayerCharacter
{
	GENERATED_BODY()
	
public:

	ARPGSorcererPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:

	/** 스킬 사용 */

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

	/** 스킬 취소 */

	virtual void CancelAbility() override;

	UFUNCTION(Server, Reliable)
	void AimingPoseOffServer();

	UFUNCTION(NetMulticast, Reliable)
	void AimingPoseOffMulticast();

	/** 일반 공격 */

	virtual void CastNormalAttack() override;

	UFUNCTION(Server, Reliable)
	void SpawnNormalProjectileServer();

	void SpawnNormalProjectile();

	/** Ability Q */

	UFUNCTION()
	void FireRestrictionBall(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void FireRestrictionBallServer();

	void SpawnRestrictionProjectile();

	/** Ability W */

	UFUNCTION()
	void MeteorliteFall(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void MeteorliteFallServer();

	void SpawnMeteorProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnMeteorlietPortalParticleMulticast(const FVector_NetQuantize& SpawnLocation, const FRotator& SpawnRotation);

	/** Ability E */

	UFUNCTION()
	void MeteorShower(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void MeteorShowerServer();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnMeteorPortalParticleMulticast();

	void MeteorShowerOn();

	void ApplyMeteorDamage();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnMeteorShowerParticleMulticast();

	void SpawnMeteorShowerParticle();

	/** Ability R */

	UFUNCTION()
	void FloatingCharacter(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void FloatingCharacterServer();

	UFUNCTION(NetMulticast, Reliable)
	void SetMovementModeToFlyMulticast();

	void StopFloatingCharacter();

	UFUNCTION()
	void SummonBlackhole(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void SpawnBlackholeServer();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnBlackholeMulticast();

	void SpawnBlackhole();

	UFUNCTION()
	void BlackholeBeamOn(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void ActivateBlackholwBeamServer();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateBlackholwBeamMulticast();

	void ActivateBlackholwBeam();

	UFUNCTION()
	void BlackholeEnd(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void SetMovementModeToWalkServer();

	UFUNCTION(NetMulticast, Reliable)
	void SetMovementModeToWalkMulticast();

protected:

	/** 스킬 사용 제한 */

	virtual void OnAbilityEnded(EPressedKey KeyType) override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	/** 일반 공격 */

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBaseProjectile> PrimaryPorjectile;

	/** Q 스킬 */

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGRestrictionProjectile> RestrictionPorjectile;

	/** W 스킬 */

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBaseProjectile> MeteorlitePorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteorlite")
	UParticleSystem* MeteorliteParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteorlite")
	UParticleSystem* MeteorlitePortalParticle;

	/** E 스킬 */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteor Shower")
	UParticleSystem* MeteorShowerParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteor Shower")
	UParticleSystem* MeteorPortalParticle;

	FTimerHandle MeteorShowerTimer;

	FTimerHandle MeteorDamageTimer;

	UPROPERTY(Replicated)
	FVector SphereTraceLocation;

	int8 MeteorDamageCount = 0;

	/** R 스킬 */

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBlackhole> BlackholeClass;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Blackhole")
	UParticleSystem* BlackholeParticle;

	UPROPERTY()
	UParticleSystemComponent* BlackholeBeamParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Blackhole")
	UParticleSystem* BlackholeBeamParticle;

	UPROPERTY(Replicated)
	bool bFloatCharacter = false;

	FTimerHandle FloatingTimer;

	UPROPERTY(EditAnywhere, Category = "Character | Attack Option")
	float AttackRange = 900.f;
};

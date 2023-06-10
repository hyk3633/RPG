
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enums/NotifyCode.h"
#include "RPGSorcererPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGBaseProjectile;
class ARPGSpeedDownProjectile;
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

public:

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

protected:

	virtual void CastNormalAttack() override;

	/** Ability Q */

	UFUNCTION()
	void FireSpeedDownBall(ENotifyCode NotifyCode);

	/** Ability W */

	UFUNCTION()
	void MeteorliteFall(ENotifyCode NotifyCode);

	/** Ability E */

	UFUNCTION()
	void MeteorShower(ENotifyCode NotifyCode);

	void SpawnMeteorShowerParticle();

	UFUNCTION()
	void OnMeteorShowerParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat);

	/** Ability R */

	UFUNCTION()
	void FloatACharacter(ENotifyCode NotifyCode);

	void DeactivateFloatingCharacter();

	UFUNCTION()
	void SummonBlackhole(ENotifyCode NotifyCode);

protected:

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBaseProjectile> PrimaryPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGSpeedDownProjectile> SpeedDownPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBaseProjectile> MeteorlitePorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGBlackhole> BlackholeClass;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteor Shower")
	UParticleSystem* MeteorPortalParticle;

	UPROPERTY()
	UParticleSystemComponent* MeteorShowerParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteor Shower")
	UParticleSystem* MeteorShowerParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteorlite")
	UParticleSystem* MeteorliteParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Meteorlite")
	UParticleSystem* MeteorlitePortalParticle;

	FTimerHandle MeteorShowerTimer;

	bool bFloatCharacter = false;

	FTimerHandle FloatingTimer;

	UPROPERTY(EditAnywhere, Category = "Character | Attack Option")
	float AttackRange = 900.f;
};

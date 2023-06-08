
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Projectile/RPGProjectile.h"
#include "Enums/NotifyCode.h"
#include "RPGSorcererPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGProjectile;

UCLASS()
class RPG_API ARPGSorcererPlayerCharacter : public ARPGBasePlayerCharacter
{
	GENERATED_BODY()
	
public:

	ARPGSorcererPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	void DrawTargetingCursor();

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

protected:

	virtual void CastNormalAttack() override;

	ARPGProjectile* SpawnProjectile(TSubclassOf<ARPGProjectile> ProjClass, const FVector& SpawnLoc, const FRotator& SpawnRot);

	/** Ability Q */

	UFUNCTION()
	void FireRestrictionBall(ENotifyCode NotifyCode);

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

	UFUNCTION()
	void BlackholeOn(ENotifyCode NotifyCode);

	UFUNCTION()
	void BlackholeOff(ENotifyCode NotifyCode);

	

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> PrimaryPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> RestrictionBallPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> MeteorlitePorjectile;

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

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Blackhole")
	UParticleSystem* BlackholeParticle;

	bool bFloatCharacter = false;

	bool bBlackholeOn = false;

	FTimerHandle FloatingTimer;

	void DeactivateFloatingCharacter();

	UPROPERTY(EditAnywhere, Category = "Character | Attack Option")
	float AttackRange = 900.f;
};


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

	UFUNCTION()
	void FireRestrictionBall(ENotifyCode NotifyCode);

	UFUNCTION()
	void MeteorliteFall(ENotifyCode NotifyCode);

	UFUNCTION()
	void MeteorShower(ENotifyCode NotifyCode);

	UFUNCTION()
	void OnMeteorShowerParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat);

	void SpawnMeteorShowerParticle();

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

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorPortalParticle;

	UPROPERTY()
	UParticleSystemComponent* MeteorShowerParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorShowerParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorliteFallingParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorliteExplosionParticle;

	FTimerHandle MeteorShowerTimer;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* BlackholeParticle;

	bool bFloatCharacter = false;

	bool bBlackholeOn = false;

	FTimerHandle FloatingTimer;

	void DeactivateFloatingCharacter();


};

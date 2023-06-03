
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Projectile/RPGProjectile.h"
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
	void FireRestrictionBall();

	UFUNCTION()
	void MeteorFall();

	UFUNCTION()
	void MeteorShower();

	UFUNCTION()
	void SummongBlackhole();

protected:

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> PrimaryPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> RestrictionBallPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Projectile")
	TSubclassOf<ARPGProjectile> MeteorPorjectile;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorPortalParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorShowerParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorFallingParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* MeteorExplosionParticle;

	FTimerHandle MeteorShowerTimer;

	void SpawnMeteorShowerParticle();

	UPROPERTY(EditAnywhere, Category = "Character | Particle")
	UParticleSystem* BlackholeParticle;



};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class RPG_API ARPGProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ARPGProjectile();

	void InitPlayerProjectile();

	void SetHomingTarget(const ACharacter* TargetCha);

	void SetThrowingMode();

	virtual void Tick(float DeltaTime) override;

	void SetExpireTime(float LifeTime);

protected:

	void ExpireProjectile();

	void DeactivateProjectile();
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	void ProcessHitEvent(const FHitResult& HitResult);

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComponent;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BodyMesh;

	UPROPERTY()
	UParticleSystemComponent* BodyParticleComp;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* BodyParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* WorldImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* CharacterImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* NoImpactParticle;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* TrailParticle;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	FTimerHandle ExpireTimer;

	UPROPERTY(EditAnywhere, Category = "Projectile | Option")
	float ExpireTime = 1.f;

	bool bIsHoming = false;
	float VelocityLastFrame;

};

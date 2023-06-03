
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

	void SetHomingMode(const ACharacter* TargetCha);

	virtual void Tick(float DeltaTime) override;

protected:
	
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

	UPROPERTY(EditAnywhere, Category = "Projectile | Option")
	float LifeSpan = 5.f;

};

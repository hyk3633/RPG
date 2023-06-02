
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class RPG_API ARPGProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ARPGProjectile();

	void InitPlayerProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;

	void ProjectileLineTrace();

	void ProcessHitEvent(const FHitResult& LTResult);

private:

	UPROPERTY()
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BodyMesh;

	UPROPERTY()
	UParticleSystemComponent* BodyParticleComp;

	UPROPERTY(EditAnywhere, Category = "Projectile | Effects")
	UParticleSystem* BodyParticle;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Projectile | Option")
	float LifeSpan = 5.f;

	ECollisionChannel ProejctileType;
};

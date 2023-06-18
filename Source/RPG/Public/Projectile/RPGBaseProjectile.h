
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGBaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

USTRUCT()
struct FProjectileData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool bIsPlayers;

	UPROPERTY()
	int32 Damage;

	UPROPERTY()
	float ExpireTime;

	UPROPERTY()
	int32 InitialSpeed;

	UPROPERTY()
	int32 CollisionRadius;

	UPROPERTY()
	bool bIsExplosive;

	UPROPERTY()
	int32 ExplosionRadius;

	FProjectileData()
	{
		bIsPlayers = false;
		bIsExplosive = false;
		Damage = 50;
		ExpireTime = 1.f;
		InitialSpeed = 5000;
		ExplosionRadius = 300;
	}

	FProjectileData(bool _IsPlayers, int32 _Damage, float _ExpireTime, int32 _InitialSpeed, int32 _CollisionRadius, bool _IsExplosive = false, int32 _ExplosionRadius = 0) : 
		bIsPlayers(_IsPlayers), 
		Damage(_Damage), 
		ExpireTime(_ExpireTime), 
		InitialSpeed(_InitialSpeed), 
		CollisionRadius(_CollisionRadius),
		bIsExplosive(_IsExplosive), 
		ExplosionRadius(_ExplosionRadius)
	{}
};

UCLASS()
class RPG_API ARPGBaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:

	ARPGBaseProjectile();

	void SetProjectileData(const FProjectileData& ProjData);

	virtual void Tick(float DeltaTime) override;

	void DeactivateProjectileToAllClients();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	virtual void ProcessHitEvent(const FHitResult& HitResult);

	void ExpireProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void DeactivateProjectileMulticast();

	void DeactivateProjectile();

protected:

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

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

	FTimerHandle ExpireTimer;

	bool bIsExplosive;
	int32 Damage;
	float ExpireTime;
	int32 ExplosionRadius;
};


#pragma once

#include "CoreMinimal.h"
#include "Structs/ProjectileInfo.h"
#include "GameFramework/Actor.h"
#include "RPGBaseProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UENUM()
enum class EParticleType : uint8
{
	EPT_CharacterImpact,
	EPT_WorldImpact,
	EPT_NoImpact,

	EPT_MAX
};

DECLARE_DELEGATE(DeactivateProjectileDelegate)

UCLASS()
class RPG_API ARPGBaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:

	ARPGBaseProjectile();

	void SetProjectileInfo(const FProjectileInfo& ProjData);

	void SetProjectileDamage(const float NewDamage);

	void ActivateProjectileToAllClients();

	void DeactivateProjectileToAllClients();

	void ReflectProjectileFromAllClients();

	DeactivateProjectileDelegate DDeactivateProjectile;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnImpact(const FHitResult& HitResult);

	virtual void ProcessHitEvent(const FHitResult& HitResult);

	void ExpireProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnParticleMulticast(EParticleType Type, const FVector_NetQuantize& SpawnLocation, const FRotator& SpawnRotation);

	UFUNCTION(NetMulticast, Reliable)
	void ActivateProjectileMulticast();

	void ActivateProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void DeactivateProjectileMulticast();

	void DeactivateProjectile();

	UFUNCTION(Server, Reliable)
	void ReflectedProjectileServer();

	void ReflectedProjectile();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	USphereComponent* CollisionComponent;

private:

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

	FProjectileInfo ProjInfo;

	int32 Damage;
};

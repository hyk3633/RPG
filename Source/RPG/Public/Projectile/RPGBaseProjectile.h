
#pragma once

#include "CoreMinimal.h"
#include "Structs/ProjectileInfo.h"
#include "Structs/ProjectileAssets.h"
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

	void SetProjectileInfo(const FProjectileInfo& NewInfo);

	void SetProjectileAssets(const FProjectileAssets& NewAssets);

protected:

	UFUNCTION()
	void OnRep_ProjAssets();

public:

	void SetProjectileDamage(const float NewDamage);

	void ActivateProjectileToAllClients();

	virtual void Tick(float DeltaTime) override;

	void DeactivateProjectileToAllClients();

	void ReflectProjectileFromAllClients();

	FORCEINLINE void SetHomingTarget(ACharacter* NewTarget) { TargetCharacter = NewTarget; }

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

	FTimerHandle ExpireTimer;

	FProjectileInfo ProjInfo;

	UPROPERTY(ReplicatedUsing = OnRep_ProjAssets)
	FProjectileAssets ProjAssets;

	int32 Damage;

	UPROPERTY()
	ACharacter* TargetCharacter;

	bool bTrackingOn = false;
};

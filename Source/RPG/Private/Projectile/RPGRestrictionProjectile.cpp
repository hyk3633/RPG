
#include "Projectile/RPGRestrictionProjectile.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFrameWork/Character.h"
#include "Kismet/KismetSystemLibrary.h"

ARPGRestrictionProjectile::ARPGRestrictionProjectile()
{
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 1000.f;
}

void ARPGRestrictionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (VelocityLastFrame < GetVelocity().Size() && !ProjectileMovementComponent->bIsHomingProjectile)
	{
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->SetVelocityInLocalSpace(GetActorRotation().Vector() * 1.f);
		ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	}
	else
	{
		VelocityLastFrame = GetVelocity().Size();
	}
}

void ARPGRestrictionProjectile::SetHomingTarget(ACharacter* HomingTarget)
{
	ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
}

void ARPGRestrictionProjectile::BeginPlay()
{
	Super::BeginPlay();

	VelocityLastFrame = GetVelocity().Size();
}

void ARPGRestrictionProjectile::ProcessHitEvent(const FHitResult& HitResult)
{
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		GetActorLocation(),
		GetActorLocation(),
		500.f,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::Persistent,
		Hits,
		true
	);
	for (FHitResult Hit : Hits)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy == nullptr) continue;
		Enemy->StopActionToAllClients();
	}

	Super::ProcessHitEvent(HitResult);
}

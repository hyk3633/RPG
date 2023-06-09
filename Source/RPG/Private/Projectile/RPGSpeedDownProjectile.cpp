
#include "Projectile/RPGSpeedDownProjectile.h"
#include "../RPG.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFrameWork/Character.h"

ARPGSpeedDownProjectile::ARPGSpeedDownProjectile()
{
	ProjectileMovementComponent->ProjectileGravityScale = 1.f;
	ProjectileMovementComponent->HomingAccelerationMagnitude = 1000.f;
}

void ARPGSpeedDownProjectile::BeginPlay()
{
	Super::BeginPlay();

	VelocityLastFrame = GetVelocity().Size();
}

void ARPGSpeedDownProjectile::Tick(float DeltaTime)
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

void ARPGSpeedDownProjectile::SetHomingTarget(ACharacter* HomingTarget)
{
	ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
}

void ARPGSpeedDownProjectile::ProcessHitEvent(const FHitResult& HitResult)
{
	Super::ProcessHitEvent(HitResult);

	// TODO : Slow down Enemy
}

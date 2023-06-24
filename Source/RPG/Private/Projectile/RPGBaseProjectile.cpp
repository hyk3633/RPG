
#include "Projectile/RPGBaseProjectile.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARPGBaseProjectile::ARPGBaseProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->MaxSpeed = 7000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->SetAutoActivate(true);
}

void ARPGBaseProjectile::SetProjectileData(const FProjectileData& ProjData)
{
	if (ProjData.bIsPlayers)
	{
		CollisionComponent->SetCollisionObjectType(ECC_PlayerProjectile);
		CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerBody, ECollisionResponse::ECR_Ignore);
	}
	else
	{
		CollisionComponent->SetCollisionObjectType(ECC_EnemyProjectile);
		CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Ignore);
	}
	Damage = ProjData.Damage;
	ExpireTime = ProjData.ExpireTime;
	InitialSpeed = ProjData.InitialSpeed;
	ProjectileMovementComponent->InitialSpeed = ProjData.InitialSpeed;
	CollisionComponent->SetSphereRadius(ProjData.CollisionRadius);
	bIsExplosive = ProjData.bIsExplosive;
	ExplosionRadius = ProjData.ExplosionRadius;
}

void ARPGBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ARPGBaseProjectile::OnImpact);

	if (BodyParticle)
	{
		BodyParticleComp = UGameplayStatics::SpawnEmitterAttached(
			BodyParticle,
			CollisionComponent,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}

	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBaseProjectile::ExpireProjectile, ExpireTime, false);
}

void ARPGBaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGBaseProjectile::OnImpact(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		ProcessHitEvent(HitResult);
	}
}

void ARPGBaseProjectile::ProcessHitEvent(const FHitResult& HitResult)
{
	DeactivateProjectileMulticast();

	ACharacter* Character = Cast<ACharacter>(HitResult.GetActor());
	if (Character)
	{
		if (CharacterImpactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CharacterImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}
		if (bIsExplosive)
		{
			UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(), TArray<AActor*>(), GetOwner(), GetInstigatorController());
		}
		else
		{
			UGameplayStatics::ApplyDamage(Character, Damage, GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		}
	}
	else
	{
		if (WorldImpactParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WorldImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
		}
	}
}

void ARPGBaseProjectile::ExpireProjectile()
{
	DeactivateProjectile();
	if (NoImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), NoImpactParticle, GetActorLocation(), GetActorRotation());
	}
}

void ARPGBaseProjectile::DeactivateProjectileToAllClients()
{
	DeactivateProjectileMulticast();
}

void ARPGBaseProjectile::DeactivateProjectileMulticast_Implementation()
{
	DeactivateProjectile();
}

void ARPGBaseProjectile::DeactivateProjectile()
{
	GetWorldTimerManager().ClearTimer(ExpireTimer);
	BodyMesh->SetVisibility(false);
	ProjectileMovementComponent->StopMovementImmediately();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (BodyParticleComp)
	{
		BodyParticleComp->Deactivate();
	}
	SetLifeSpan(1.f);
}

void ARPGBaseProjectile::ReflectProjectileFromAllClients()
{
	ReflectedProjectileMulticast();
}

void ARPGBaseProjectile::ReflectedProjectileMulticast_Implementation()
{
	ReflectedProjectile();
}

void ARPGBaseProjectile::ReflectedProjectile()
{
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMovementComponent->Deactivate();
	CollisionComponent->SetCollisionObjectType(ECC_PlayerProjectile);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerBody, ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Block);
	SetActorRotation((GetOwner()->GetActorLocation() - GetActorLocation()).Rotation());
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(1000, 0, 0));
	ProjectileMovementComponent->Activate();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetWorldTimerManager().ClearTimer(ExpireTimer);
	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBaseProjectile::ExpireProjectile, ExpireTime, false);
}

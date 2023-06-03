
#include "Projectile/RPGProjectile.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ARPGProjectile::ARPGProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_EnemyProjectile);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 7000.f;
	ProjectileMovementComponent->MaxSpeed = 7000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->SetAutoActivate(true);

	SetLifeSpan(LifeSpan);
}

void ARPGProjectile::InitPlayerProjectile()
{
	CollisionComponent->SetCollisionObjectType(ECC_PlayerProjectile);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerBody, ECollisionResponse::ECR_Ignore);
}

void ARPGProjectile::SetHomingMode(const ACharacter* TargetCha)
{
	ProjectileMovementComponent->bIsHomingProjectile = true;
	ProjectileMovementComponent->InitialSpeed = 100.f;
	ProjectileMovementComponent->MaxSpeed = 7000.f;
	ProjectileMovementComponent->HomingTargetComponent = TargetCha->GetRootComponent();
	ProjectileMovementComponent->HomingAccelerationMagnitude = 1000.f;
}

void ARPGProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ARPGProjectile::OnImpact);
	
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
}

void ARPGProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGProjectile::OnImpact(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		ProcessHitEvent(HitResult);
	}
}

void ARPGProjectile::ProcessHitEvent(const FHitResult& HitResult)
{
	PLOG(TEXT("%s"), *HitResult.GetActor()->GetName());
	/*ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(HitResult.GetActor());
	if (Player)
	{

	}
	else
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(HitResult.GetActor());
		if (Enemy == nullptr) return;
	}*/
	// TODO : ApplyDamage
	// TODO : ÀÌÆåÆ® Ã³¸®
	if (BodyParticleComp)
	{
		BodyParticleComp->Deactivate();
	}
	if (WorldImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WorldImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
	Destroy();
}


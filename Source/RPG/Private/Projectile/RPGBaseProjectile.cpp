
#include "Projectile/RPGBaseProjectile.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

ARPGBaseProjectile::ARPGBaseProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetVisibility(false);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->SetAutoActivate(false);
}

void ARPGBaseProjectile::SetProjectileInfo(const FProjectileInfo& NewInfo)
{
	ProjInfo = NewInfo;
	if (ProjInfo.bIsPlayers)
	{
		CollisionComponent->SetCollisionProfileName(FName("PlayerProjectileProfile"));
	}
	else
	{
		CollisionComponent->SetCollisionProfileName(FName("EnemyProjectileProfile"));
	}
	CollisionComponent->SetSphereRadius(ProjInfo.CollisionRadius);
}

void ARPGBaseProjectile::SetProjectileAssets(const FProjectileAssets& NewAssets)
{
	ProjAssets = NewAssets;
}

void ARPGBaseProjectile::OnRep_ProjAssets()
{
	if (ProjAssets.BodyMesh)
	{
		BodyMesh->SetStaticMesh(ProjAssets.BodyMesh);
		BodyMesh->SetRelativeRotation(FRotator(0, -90, 0));
		BodyMesh->SetRelativeScale3D(FVector(2, 1, 2));
	}
	if (ProjAssets.BodyParticle)
	{
		BodyParticleComp = UGameplayStatics::SpawnEmitterAttached
		(
			ProjAssets.BodyParticle,
			CollisionComponent,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false,
			EPSCPoolMethod::None,
			false
		);
		BodyParticleComp->Deactivate();
	}
}

void ARPGBaseProjectile::SetProjectileDamage(const float NewDamage)
{
	Damage = NewDamage;
}

void ARPGBaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ARPGBaseProjectile::OnImpact);
	}
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
	DeactivateProjectileToAllClients();

	if (ProjInfo.bIsExplosive)
	{
		UGameplayStatics::ApplyRadialDamage(this, Damage, GetActorLocation(), ProjInfo.ExplosionRadius, ProjInfo.DamageType, TArray<AActor*>(), GetOwner(), GetInstigatorController());
	}
	else
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), Damage, GetInstigatorController(), GetOwner(), ProjInfo.DamageType);
	}

	ACharacter* Character = Cast<ACharacter>(HitResult.GetActor());
	if (Character)
	{
		SpawnEffectMulticast(EParticleType::EPT_CharacterImpact, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
	else
	{
		SpawnEffectMulticast(EParticleType::EPT_WorldImpact, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
	}
}

void ARPGBaseProjectile::ActivateProjectileToAllClients()
{
	ActivateProjectile();
	ActivateProjectileMulticast();
}

void ARPGBaseProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGBaseProjectile::ActivateProjectileMulticast_Implementation()
{
	if (!HasAuthority())
	{
		if (BodyParticleComp) BodyParticleComp->Activate();
		BodyMesh->SetVisibility(true);
		bActivated = true;
	}
}

void ARPGBaseProjectile::ActivateProjectile()
{
	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBaseProjectile::ExpireProjectile, ProjInfo.ExpireTime, false);

	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(ProjInfo.InitialSpeed, 0, 0));
	ProjectileMovementComponent->Activate();

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (ProjInfo.bIsHoming && IsValid(TargetCharacter))
	{
		ProjectileMovementComponent->bIsHomingProjectile = true;
		ProjectileMovementComponent->HomingTargetComponent = TargetCharacter->GetCapsuleComponent();
		ProjectileMovementComponent->HomingAccelerationMagnitude = 3000.f;
	}
}

void ARPGBaseProjectile::ExpireProjectile()
{
	SpawnEffectMulticast(EParticleType::EPT_NoImpact, GetActorLocation(), GetActorRotation());
	DeactivateProjectileToAllClients();
}

void ARPGBaseProjectile::SpawnEffectMulticast_Implementation(EParticleType Type, const FVector_NetQuantize& SpawnLocation, const FRotator& SpawnRotation)
{
	if (HasAuthority()) return;

	if (Type == EParticleType::EPT_CharacterImpact)
	{
		if(ProjAssets.CharacterImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjAssets.CharacterImpactParticle, SpawnLocation, SpawnRotation);
		if(ProjAssets.ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ProjAssets.ImpactSound, SpawnLocation);
	}
	else if (Type == EParticleType::EPT_WorldImpact)
	{
		if(ProjAssets.WorldImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjAssets.WorldImpactParticle, SpawnLocation, SpawnRotation);
		if (ProjAssets.ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ProjAssets.ImpactSound, SpawnLocation);
		}
	}
	else if (Type == EParticleType::EPT_NoImpact)
	{
		if(ProjAssets.NoImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjAssets.NoImpactParticle, SpawnLocation, SpawnRotation);
	}
}

void ARPGBaseProjectile::DeactivateProjectileToAllClients()
{
	DeactivateProjectile();
	DeactivateProjectileMulticast();
}

void ARPGBaseProjectile::DeactivateProjectileMulticast_Implementation()
{
	if (!HasAuthority())
	{
		BodyMesh->SetVisibility(false);
		if (BodyParticleComp) BodyParticleComp->Deactivate();
		bActivated = false;
	}
}

void ARPGBaseProjectile::DeactivateProjectile()
{
	GetWorldTimerManager().ClearTimer(ExpireTimer);
	ProjectileMovementComponent->Deactivate();
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DDeactivateProjectile.ExecuteIfBound();

	if (ProjInfo.bIsHoming && TargetCharacter)
	{
		ProjectileMovementComponent->bIsHomingProjectile = false;
		TargetCharacter = nullptr;
	}
}

void ARPGBaseProjectile::ReflectProjectileFromAllClients()
{
	ReflectedProjectileServer();
}

void ARPGBaseProjectile::ReflectedProjectileServer_Implementation()
{
	ReflectedProjectile();
}

void ARPGBaseProjectile::ReflectedProjectile()
{
	ProjectileMovementComponent->Deactivate();

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComponent->SetCollisionObjectType(ECC_PlayerProjectile);
	CollisionComponent->SetCollisionResponseToChannel(ECC_PlayerBody, ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Block);

	SetActorRotation((GetOwner()->GetActorLocation() - GetActorLocation()).Rotation());

	ProjectileMovementComponent->SetUpdatedComponent(RootComponent);
	ProjectileMovementComponent->SetVelocityInLocalSpace(FVector(1000, 0, 0));
	ProjectileMovementComponent->Activate();

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GetWorldTimerManager().ClearTimer(ExpireTimer);
	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBaseProjectile::ExpireProjectile, ProjInfo.ExpireTime, false);
}

void ARPGBaseProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBaseProjectile, ProjAssets);
}



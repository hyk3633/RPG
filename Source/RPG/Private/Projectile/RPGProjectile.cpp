
#include "Projectile/RPGProjectile.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ARPGProjectile::ARPGProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Comp"));
	SetRootComponent(RootComp);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body Mesh"));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetupAttachment(RootComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = 7000.f;
	ProjectileMovementComponent->MaxSpeed = 7000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->SetAutoActivate(true);

	ProejctileType = ECC_EnemyProjectile;
	SetLifeSpan(LifeSpan);

	// box component 로 바꾸기
}

void ARPGProjectile::InitPlayerProjectile()
{
	ProejctileType = ECC_PlayerProjectile;
}

void ARPGProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (BodyParticle)
	{
		BodyParticleComp = UGameplayStatics::SpawnEmitterAttached(
			BodyParticle,
			RootComp,
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

	ProjectileLineTrace();
}

void ARPGProjectile::ProjectileLineTrace()
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorForwardVector() * 30.f, ProejctileType);
	if (HitResult.bBlockingHit)
	{
		ProcessHitEvent(HitResult);
	}
}

void ARPGProjectile::ProcessHitEvent(const FHitResult& LTResult)
{
	if (ProejctileType == ECC_PlayerProjectile)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(LTResult.GetActor());
		if (Enemy == nullptr) return;

	}
	else
	{
		ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(LTResult.GetActor());
		if (Player == nullptr) return;
	}
	// TODO : ApplyDamage
	PLOG(TEXT("%s"), *LTResult.GetActor()->GetName());
	Destroy();
	// TODO : 이펙트 처리
}


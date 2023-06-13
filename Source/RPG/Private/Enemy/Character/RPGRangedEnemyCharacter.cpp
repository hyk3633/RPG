
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Projectile/RPGBaseProjectile.h"
#include "../RPG.h"
#include "Engine/SkeletalMeshSocket.h"

ARPGRangedEnemyCharacter::ARPGRangedEnemyCharacter()
{
	BowMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Bow Mesh"));
	BowMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("Socket_Bow"));
}

void ARPGRangedEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPGRangedEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ARPGRangedEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARPGRangedEnemyCharacter::Attack()
{
	AnimNotify_LineTraceOnSocket();
}

void ARPGRangedEnemyCharacter::AnimNotify_LineTraceOnSocket()
{
	const USkeletalMeshSocket* BowSocket = BowMesh->GetSocketByName(FName("Bow_Root"));
	if (BowSocket == nullptr) return;

	const FVector TraceStart = BowSocket->GetSocketLocation(BowMesh);
	FVector TraceEnd = TraceStart;
	APawn* TargetToAttack = MyController->GetTarget();
	if (TargetToAttack)
	{
		TraceEnd += (TargetToAttack->GetActorLocation() - TraceStart);
	}
	else
	{
		TraceEnd += GetActorForwardVector() * 15000.f;
	}

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_EnemyAttack);

	if (HitResult.bBlockingHit)
	{
		FRotator FireRotation = (HitResult.ImpactPoint - TraceStart).Rotation();
		FireRotation.Pitch = 0.f;
		SpawnProjectile(TraceStart, FireRotation);
	}
}

void ARPGRangedEnemyCharacter::SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (ProjectileClass == nullptr) return;
	
	const FTransform SpawnTransform = FTransform(SpawnRotation, SpawnLocation);
	ARPGBaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARPGBaseProjectile>(ProjectileClass, SpawnTransform, this, this);
	if (Projectile)
	{
		Projectile->SetProjectileData(FProjectileData(false, 50, 1, 1000, 8));
		Projectile->FinishSpawning(SpawnTransform);
	}
}


#include "Projectile/RPGRestrictionProjectile.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFrameWork/Character.h"
#include "Kismet/KismetSystemLibrary.h"

ARPGRestrictionProjectile::ARPGRestrictionProjectile()
{
	
}

void ARPGRestrictionProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGRestrictionProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGRestrictionProjectile::ProcessHitEvent(const FHitResult& HitResult)
{
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		GetActorLocation(),
		GetActorLocation(),
		300.f,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
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

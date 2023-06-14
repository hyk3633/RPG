

#include "Enemy/Character/RPGMeleeEnemyCharacter.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "../RPG.h"
#include "Kismet/GameplayStatics.h"

ARPGMeleeEnemyCharacter::ARPGMeleeEnemyCharacter()
{

}

void ARPGMeleeEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPGMeleeEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ARPGMeleeEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ARPGMeleeEnemyCharacter::Attack()
{
	FVector TraceStart = GetActorLocation();
	TraceStart.Z += 50.f;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceStart + GetActorForwardVector() * 150.f, ECC_EnemyAttack);
	DrawDebugLine(GetWorld(), TraceStart, TraceStart + GetActorForwardVector() * 200.f, FColor::Red, false, 3.f, 0U, 2.f);

	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), 50.f, Controller, this, UDamageType::StaticClass());
	}
}


#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enums/EnemyAttackType.h"
#include "../RPG.h"
#include "Kismet/KismetSystemLibrary.h"

ARPGRangedEnemyCharacter::ARPGRangedEnemyCharacter()
{
	
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

void ARPGRangedEnemyCharacter::InitAnimInstance()
{
	Super::InitAnimInstance();

	MyAnimInst->DOnRangedAttack.AddUFunction(this, FName("RangedAttack"));
}

void ARPGRangedEnemyCharacter::BTTask_RangedAttack()
{
	PlayRangedAttackEffectMulticast();
}

bool ARPGRangedEnemyCharacter::CheckCanFireToTarget()
{
	FHitResult Hit;
	FVector Loc = GetActorLocation();
	Loc.Z = 60.f;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Loc,
		GetTarget()->GetTargetLocation(),
		FVector(20, 20, 20),
		GetActorForwardVector().Rotation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::ForOneFrame,
		Hit,
		true,
		FColor::Red,
		FColor::Green,
		5.f
	);
	return !Hit.bBlockingHit;
}

void ARPGRangedEnemyCharacter::PlayRangedAttackEffectMulticast_Implementation()
{
	if (MyAnimInst == nullptr) return;
	MyAnimInst->PlayRangedAttackMontage();
}

void ARPGRangedEnemyCharacter::RangedAttack()
{
	if (HasAuthority())
	{
		EnemyForm->RangedAttack(this, GetTarget());
	}
}

bool ARPGRangedEnemyCharacter::ShouldIStopMovement()
{
	const float DistToTarget = GetDistanceTo(GetTarget());
	// 타겟과의 거리가 감지 거리보다 멀다면 계속 움직이기
	if (DistToTarget > GetDetectDistance())
	{
		return false;
	}
	// 타겟과의 거리가 감지 거리 이내라면
	else
	{
		// 타겟과의 거리가 공격 거리보단 멀다면
		if (DistToTarget > GetAttackDistance())
		{
			// 사격 가능한 위치라면 멈추고 그렇지 않으면 계속 움직이기
			return CheckCanFireToTarget();
		}
		else
		{
			// 타겟과의 거리가 공격 거리 이내라면 멈추기
			return true;
		}
	}
}

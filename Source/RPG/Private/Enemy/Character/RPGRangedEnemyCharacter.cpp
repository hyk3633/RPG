
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
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		GetActorLocation(),
		GetTarget()->GetTargetLocation(),
		FVector(20, 20, 20),
		GetActorForwardVector().Rotation(),
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
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
	MyAnimInst->PlayRangedAttackMontage(HasAuthority());
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
		// 원/근거리 적
		if (GetAttackType() == EEnemyAttackType::EEAT_Hybrid)
		{
			// 사격 가능한 위치 또는 타겟과의 거리가 근접 공격 거리 이내라면
			if (CheckCanFireToTarget() || DistToTarget <= GetAttackDistance())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		// 원거리 적
		else
		{
			// 공격 거리 이내면서 사격 가능한 위치이면 멈추기
			if (DistToTarget <= GetAttackDistance() && CheckCanFireToTarget())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
}


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
	// Ÿ�ٰ��� �Ÿ��� ���� �Ÿ����� �ִٸ� ��� �����̱�
	if (DistToTarget > GetDetectDistance())
	{
		return false;
	}
	// Ÿ�ٰ��� �Ÿ��� ���� �Ÿ� �̳����
	else
	{
		// ��/�ٰŸ� ��
		if (GetAttackType() == EEnemyAttackType::EEAT_Hybrid)
		{
			// ��� ������ ��ġ �Ǵ� Ÿ�ٰ��� �Ÿ��� ���� ���� �Ÿ� �̳����
			if (CheckCanFireToTarget() || DistToTarget <= GetAttackDistance())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		// ���Ÿ� ��
		else
		{
			// ���� �Ÿ� �̳��鼭 ��� ������ ��ġ�̸� ���߱�
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

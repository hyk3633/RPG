
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enums/EnemyAttackType.h"
#include "../RPG.h"

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

void ARPGRangedEnemyCharacter::BTTask_Attack()
{
	if (AttackType == EEnemyAttackType::EEAT_Hybrid && GetDistanceTo(MyController->GetTarget()) < 100.f)
	{
		PlayMeleeAttackEffectMulticast();
	}
	else
	{
		PlayRangedAttackEffectMulticast();
	}
}

void ARPGRangedEnemyCharacter::PlayRangedAttackEffectMulticast_Implementation()
{
	if (MyAnimInst == nullptr) return;
	MyAnimInst->PlayRangedAttackMontage();
}

void ARPGRangedEnemyCharacter::Attack()
{
	if (EnemyForm && HasAuthority())
	{
		if (AttackType == EEnemyAttackType::EEAT_Hybrid)
		{
			EnemyForm->MeleeAttack(this);
		}
		else
		{
			EnemyForm->RangedAttack(this);
		}
	}
}
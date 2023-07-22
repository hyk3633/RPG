
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
		PlayMeleeAttackMontageMulticast();
	}
	else
	{
		PlayRangedAttackMontageMulticast();
	}
}

void ARPGRangedEnemyCharacter::PlayRangedAttackMontageMulticast_Implementation()
{
	if (MyAnimInst == nullptr) return;
	MyAnimInst->PlayRangedAttackMontage();
}

void ARPGRangedEnemyCharacter::Attack()
{
	if (EnemyForm == nullptr) return;

	if (AttackType == EEnemyAttackType::EEAT_Hybrid)
	{
		EnemyForm->MeleeAttack(this);
	}
	else
	{
		EnemyForm->RangedAttack(this);
	}
}
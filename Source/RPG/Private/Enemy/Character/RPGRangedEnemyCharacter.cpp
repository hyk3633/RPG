
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

void ARPGRangedEnemyCharacter::InitAnimInstance()
{
	Super::InitAnimInstance();

	MyAnimInst->DOnRangedAttack.AddUFunction(this, FName("RangedAttack"));
}

void ARPGRangedEnemyCharacter::BTTask_Attack()
{
	if (AttackType == EEnemyAttackType::EEAT_Hybrid && GetDistanceTo(MyController->GetTarget()) <= GetAttackDistance())
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

void ARPGRangedEnemyCharacter::RangedAttack()
{
	if (HasAuthority())
	{
		EnemyForm->RangedAttack(this, GetTarget());
	}
}

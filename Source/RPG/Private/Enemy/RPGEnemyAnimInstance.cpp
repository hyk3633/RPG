

#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"

void URPGEnemyAnimInstance::PlayAttackMontage()
{
	if (AttackMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	
	Montage_Play(AttackMontage);
}

void URPGEnemyAnimInstance::PlayDeathMontage()
{
	if (DeathMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	Montage_Play(DeathMontage);
}

void URPGEnemyAnimInstance::PlayGetupMontage()
{
	if (GetupMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	Montage_Play(GetupMontage);
}

void URPGEnemyAnimInstance::AnimNotify_Attack()
{
	DOnAttack.Broadcast();
}
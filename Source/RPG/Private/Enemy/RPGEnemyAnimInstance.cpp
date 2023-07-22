

#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"

void URPGEnemyAnimInstance::BindFunction()
{
	OnMontageEnded.AddDynamic(this, &URPGEnemyAnimInstance::OnAttackMontageEnded);
}

void URPGEnemyAnimInstance::PlayMeleeAttackMontage()
{
	if (MeleeAttackMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	
	Montage_Play(MeleeAttackMontage);
}

void URPGEnemyAnimInstance::PlayRangedAttackMontage()
{
	if (RangedAttackMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;

	Montage_Play(RangedAttackMontage);
}

void URPGEnemyAnimInstance::PlayDeathMontage()
{
	if (DeathMontage == nullptr) return;
	Montage_Play(DeathMontage);
}

void URPGEnemyAnimInstance::PlayGetupMontage()
{
	if (GetupMontage == nullptr) return;
	Montage_Play(GetupMontage);
}

void URPGEnemyAnimInstance::CancelMontage()
{
	Montage_Stop(0.f, DeathMontage);
}

void URPGEnemyAnimInstance::AnimNotify_Attack()
{
	DOnAttack.Broadcast();
}

void URPGEnemyAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MeleeAttackMontage || Montage == RangedAttackMontage)
	{
		DOnAttackEnded.Broadcast();
	}
}

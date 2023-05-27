

#include "Enemy/RPGEnemyAnimInstance.h"

void URPGEnemyAnimInstance::PlayAttackMontage()
{
	if (AttackMontage == nullptr) return;
	if (Montage_IsPlaying(AttackMontage)) return;
	Montage_Play(AttackMontage);
}

void URPGEnemyAnimInstance::PlayDeathMontage()
{
	if (DeathMontage == nullptr) return;
	if (Montage_IsPlaying(DeathMontage)) return;
	Montage_Play(DeathMontage);
}

void URPGEnemyAnimInstance::AnimNotify_AttackTrace()
{
	DOnAttackTrace.Broadcast();
}

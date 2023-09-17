

#include "Enemy/RPGEnemyAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"

void URPGEnemyAnimInstance::BindFunction()
{
	OnMontageEnded.AddDynamic(this, &URPGEnemyAnimInstance::OnAttackMontageEnded);
	OnMontageEnded.AddDynamic(this, &URPGEnemyAnimInstance::OnHitReactionMontageEnded);
}

void URPGEnemyAnimInstance::PlayMeleeAttackMontage(const bool HasAuthority)
{
	if (IsAnyMontagePlaying()) return;
	if (HasAuthority)
	{
		if (ServerMeleeAttackMontage == nullptr) return;
		Montage_Play(ServerMeleeAttackMontage, 1.25f);
	}
	else
	{
		if (ClientMeleeAttackMontage == nullptr) return;
		Montage_Play(ClientMeleeAttackMontage, 1.25f);
	}	
}

void URPGEnemyAnimInstance::PlayRangedAttackMontage(const bool HasAuthority)
{
	if (IsAnyMontagePlaying()) return;
	if (HasAuthority)
	{
		if (ServerRangedAttackMontage == nullptr) return;
		Montage_Play(ServerRangedAttackMontage, 1.25f);
	}
	else
	{
		if (ClientRangedAttackMontage == nullptr) return;
		Montage_Play(ClientRangedAttackMontage, 1.25f);
	}
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

void URPGEnemyAnimInstance::PlayHitReactionMontage()
{
	if (HitReactionMontage == nullptr) return;
	if (Montage_IsPlaying(GetupMontage)) return;
	Montage_Play(HitReactionMontage, 1.5f);
}

void URPGEnemyAnimInstance::CancelMontage()
{
	Montage_Stop(0.f, DeathMontage);
}

void URPGEnemyAnimInstance::AnimNotify_Attack()
{
	DOnAttack.Broadcast();
}

void URPGEnemyAnimInstance::AnimNotify_RangedAttack()
{
	DOnRangedAttack.Broadcast();
}

void URPGEnemyAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ServerMeleeAttackMontage || Montage == ServerRangedAttackMontage ||
	    Montage == ClientMeleeAttackMontage || Montage == ClientRangedAttackMontage)
	{
		DOnAttackEnded.Broadcast();
	}
}

void URPGEnemyAnimInstance::OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == HitReactionMontage)
	{
		DOnHitReactionEnded.Broadcast();
	}
}


#include "Enemy/Boss/RPGBossEnemyAnimInstance.h"
#include "../RPG.h"

void URPGBossEnemyAnimInstance::BindBossEnemyFunction()
{
	OnMontageEnded.AddDynamic(this, &URPGBossEnemyAnimInstance::OnSpecialAttackMontageEnded);
}

void URPGBossEnemyAnimInstance::PlayEmitShockWaveMontage(const bool HasAuthority)
{
	if (IsAnyMontagePlaying()) return;
	if (HasAuthority)
	{
		if (ServerEmitShockWaveMontage == nullptr) return;
		Montage_Play(ServerEmitShockWaveMontage);
	}
	else
	{
		if (ClientEmitShockWaveMontage == nullptr) return;
		Montage_Play(ClientEmitShockWaveMontage);
	}
}

void URPGBossEnemyAnimInstance::PlayFireMortarMontage(const bool HasAuthority)
{
	if (IsAnyMontagePlaying()) return;
	if (HasAuthority)
	{
		if (ServerFireMortarMontage == nullptr) return;
		Montage_Play(ServerFireMortarMontage);
	}
	else
	{
		if (ClientFireMortarMontage == nullptr) return;
		Montage_Play(ClientFireMortarMontage);
	}
}

void URPGBossEnemyAnimInstance::PlayBulldozeMontage(const bool HasAuthority)
{
	if (IsAnyMontagePlaying()) return;
	if (HasAuthority)
	{
		if (ServerBulldozeMontage == nullptr) return;
		Montage_Play(ServerBulldozeMontage);
	}
	else
	{
		if (ClientBulldozeMontage == nullptr) return;
		Montage_Play(ClientBulldozeMontage);
	}
}

bool URPGBossEnemyAnimInstance::GetIsTurning() const
{
	if (GetCurveValue(TEXT("Turning")) != 0.f) return true;
	else return false;
}

void URPGBossEnemyAnimInstance::AnimNotify_SpecialAttack(ESpecialAttackType Type)
{
	DOnSpecialAttack.Broadcast(Type);
}

void URPGBossEnemyAnimInstance::OnSpecialAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == ServerEmitShockWaveMontage || Montage == ServerFireMortarMontage || Montage == ServerBulldozeMontage ||
		Montage == ClientEmitShockWaveMontage || Montage == ClientFireMortarMontage || Montage == ClientBulldozeMontage)
	{
		DOnSpecialAttackEnd.Broadcast();
	}
}

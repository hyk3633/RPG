
#include "Enemy/Boss/RPGBossEnemyAnimInstance.h"
#include "../RPG.h"



void URPGBossEnemyAnimInstance::BindFunction()
{
	Super::BindFunction();
	OnMontageEnded.AddDynamic(this, &URPGBossEnemyAnimInstance::OnSpecialAttackMontageEnded);
}

void URPGBossEnemyAnimInstance::PlayEmitShockWaveMontage()
{
	if (EmitShockWaveMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;

	Montage_Play(EmitShockWaveMontage);
}

void URPGBossEnemyAnimInstance::PlayFireMortarMontage()
{
	if (FireMortarMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;

	Montage_Play(FireMortarMontage);
}

void URPGBossEnemyAnimInstance::PlayBulldozeMontage()
{
	if (BulldozeMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;

	Montage_Play(BulldozeMontage);
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
	if (Montage == EmitShockWaveMontage || Montage == FireMortarMontage || Montage == BulldozeMontage)
	{
		DOnSpecialAttackEnd.Broadcast();
	}
}

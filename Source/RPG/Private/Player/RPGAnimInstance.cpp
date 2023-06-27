

#include "Player/RPGAnimInstance.h"
#include "../RPG.h"

void URPGAnimInstance::NativeInitializeAnimation()
{
	OnMontageEnded.AddDynamic(this, &URPGAnimInstance::OnAttackMontageEnded);
	OnMontageEnded.AddDynamic(this, &URPGAnimInstance::OnDeathMontageEnded);
	OnMontageEnded.AddDynamic(this, &URPGAnimInstance::OnAbilityMontageEnded);
}

void URPGAnimInstance::PlayNormalAttackMontage()
{
	if (NormalAttackMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	Montage_Play(NormalAttackMontage);
}

void URPGAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), NormalAttackMontage);
}

void URPGAnimInstance::PlayAbilityMontageOfKey(bool bJumpToSection)
{
	switch (CurrentKeyState)
	{
	case EPressedKey::EPK_Q:
		PlayAbilityMontage(Ability_Q_Montage, bJumpToSection);
		break;
	case EPressedKey::EPK_W:
		PlayAbilityMontage(Ability_W_Montage, bJumpToSection);
		break;
	case EPressedKey::EPK_E:
		PlayAbilityMontage(Ability_E_Montage, bJumpToSection);
		break;
	case EPressedKey::EPK_R:
		PlayAbilityMontage(Ability_R_Montage, bJumpToSection);
		break;
	}
}

void URPGAnimInstance::PlayAbilityMontage(UAnimMontage* AbilityMontage, bool bJumpToSection)
{
	if (AbilityMontage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	Montage_Play(AbilityMontage);
	if (bJumpToSection) Montage_JumpToSection(FName("Cast"), AbilityMontage);
}

bool URPGAnimInstance::GetIsAbilityERMontagePlaying()
{
	return Montage_IsPlaying(Ability_E_Montage) || Montage_IsPlaying(Ability_R_Montage);
}

void URPGAnimInstance::PlayReflectMontage()
{
	if (Ability_W_Montage == nullptr) return;
	if (IsAnyMontagePlaying()) return;
	Montage_Play(Ability_W_Montage);
	Montage_JumpToSection(FName("Reflect"), Ability_W_Montage);
}

void URPGAnimInstance::PlayDeathMontage()
{
	if (DeathMontage == nullptr) return;
	Montage_Play(DeathMontage);
}

void URPGAnimInstance::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == NormalAttackMontage)
	{
		DOnAttackEnded.Broadcast();
	}
}

void URPGAnimInstance::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DeathMontage)
	{
		DOnDeathEnded.Broadcast();
	}
}

void URPGAnimInstance::OnAbilityMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == Ability_Q_Montage)
	{
		DOnAbilityMontageEnded.Broadcast(EPressedKey::EPK_Q);
	}
	else if (Montage == Ability_W_Montage)
	{
		DOnAbilityMontageEnded.Broadcast(EPressedKey::EPK_W);
	}
	else if (Montage == Ability_E_Montage)
	{
		DOnAbilityMontageEnded.Broadcast(EPressedKey::EPK_E);
	}
	else if (Montage == Ability_R_Montage)
	{
		DOnAbilityMontageEnded.Broadcast(EPressedKey::EPK_R);
	}
}

void URPGAnimInstance::AnimNotify_AttackInputCheck()
{
	DOnAttackInputCheck.Broadcast();
}

void URPGAnimInstance::AnimNotify_MontageNotify(ENotifyCode NotifyCode)
{
	DMontageNotify.Broadcast(NotifyCode);
}

FName URPGAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	if (FMath::IsWithinInclusive<int32>(Section, (int8)1, MaxCombo))
		return FName(*FString::Printf(TEXT("A%d"), Section));
	else
		return FName("A1");
}

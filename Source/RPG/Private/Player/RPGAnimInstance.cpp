

#include "Player/RPGAnimInstance.h"
#include "../RPG.h"

void URPGAnimInstance::PlayNormalAttackMontage()
{
	if (NormalAttackMontage == nullptr) return;
	if (Montage_IsPlaying(NormalAttackMontage)) return;
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

void URPGAnimInstance::PlayAbilityMontage(UAnimMontage* AbilityMontage, bool bJumpToSection = false)
{
	if (AbilityMontage == nullptr) return;
	if (Montage_IsPlaying(AbilityMontage)) return;
	Montage_Play(AbilityMontage);
	if (bJumpToSection) Montage_JumpToSection(FName("Cast"), AbilityMontage);
}

void URPGAnimInstance::PlayDeathMontage()
{

}

void URPGAnimInstance::AnimNotify_AttackInputCheck()
{
	DOnAttackInputCheck.Broadcast();
}

void URPGAnimInstance::AnimNotify_Ability_Q_Cast()
{
	DOnAbility_Q_Cast.ExecuteIfBound();
}

void URPGAnimInstance::AnimNotify_Ability_W_Cast()
{
	DOnAbility_W_Cast.ExecuteIfBound();
}

void URPGAnimInstance::AnimNotify_Ability_E_Cast()
{
	DOnAbility_E_Cast.ExecuteIfBound();
}

void URPGAnimInstance::AnimNotify_Ability_R_Cast()
{
	DOnAbility_R_Cast.ExecuteIfBound();
}

FName URPGAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	if (FMath::IsWithinInclusive<int32>(Section, (int8)1, MaxCombo))
		return FName(*FString::Printf(TEXT("A%d"), Section));
	else
		return FName("A1");
}

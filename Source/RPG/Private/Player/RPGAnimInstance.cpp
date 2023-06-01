

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

void URPGAnimInstance::PlayAbilityMontage(EPressedKey KeyType)
{
	switch (KeyType)
	{
	case EPressedKey::EPK_Q:
		if (Ability_Q_Montage == nullptr) return;
		if (Montage_IsPlaying(Ability_Q_Montage)) return;
		Montage_Play(Ability_Q_Montage);
		break;
	case EPressedKey::EPK_W:
		if (Ability_W_Montage == nullptr) return;
		if (Montage_IsPlaying(Ability_W_Montage)) return;
		Montage_Play(Ability_W_Montage);
		break;
	case EPressedKey::EPK_E:
		if (Ability_E_Montage == nullptr) return;
		if (Montage_IsPlaying(Ability_E_Montage)) return;
		Montage_Play(Ability_E_Montage);
		break;
	case EPressedKey::EPK_R:
		if (Ability_R_Montage == nullptr) return;
		if (Montage_IsPlaying(Ability_R_Montage)) return;
		Montage_Play(Ability_R_Montage);
		break;
	}
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
	if (FMath::IsWithinInclusive<int32>(Section, 1, 4))
		return FName(*FString::Printf(TEXT("A%d"), Section));
	else
		return FName("A1");
}

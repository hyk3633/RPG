

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

void URPGAnimInstance::PlayAbility_Q_Montage()
{
	if (Ability_Q_Montage == nullptr) return;
	if (Montage_IsPlaying(Ability_Q_Montage)) return;
	Montage_Play(Ability_Q_Montage);
}

void URPGAnimInstance::PlayAbility_W_Montage()
{
	if (Ability_W_Montage == nullptr) return;
	if (Montage_IsPlaying(Ability_W_Montage)) return;
	Montage_Play(Ability_W_Montage);
}

void URPGAnimInstance::PlayAbility_E_Montage()
{
	if (Ability_E_Montage == nullptr) return;
	if (Montage_IsPlaying(Ability_E_Montage)) return;
	Montage_Play(Ability_E_Montage);
}

void URPGAnimInstance::PlayAbility_R_Montage()
{
	if (Ability_R_Montage == nullptr) return;
	if (Montage_IsPlaying(Ability_R_Montage)) return;
	Montage_Play(Ability_R_Montage);
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

FName URPGAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	if (FMath::IsWithinInclusive<int32>(Section, 1, 4))
		return FName(*FString::Printf(TEXT("A%d"), Section));
	else
		return FName("A1");
}

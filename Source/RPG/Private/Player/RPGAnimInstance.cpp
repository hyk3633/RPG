

#include "Player/RPGAnimInstance.h"
#include "../RPG.h"

void URPGAnimInstance::PlayDeathMontage()
{

}

void URPGAnimInstance::PlayNormalAttackMontage()
{
	if (NormalAttackMontage == nullptr) return;
	Montage_Play(NormalAttackMontage);
}

void URPGAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), NormalAttackMontage);
}

void URPGAnimInstance::AnimNotify_AttackInputCheck()
{
	DOnAttackInputCheck.Broadcast();
}

FName URPGAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	if (FMath::IsWithinInclusive<int32>(Section, 1, 4))
		return FName(*FString::Printf(TEXT("A%d"), Section));
	else
		return FName("A1");
}

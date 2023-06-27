

#include "Player/AnimInstance/RPGWarriorAnimInstance.h"

void URPGWarriorAnimInstance::PlayReflectMontage()
{
	if (IsAnyMontagePlaying()) return;
	Montage_Play(ReflectMontage);
}

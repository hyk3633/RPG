
#include "Player/AnimInstance/RPGSorcererAnimInstance.h"

void URPGSorcererAnimInstance::PlayAbilityIntroMontage()
{
	switch (GetCurrentKeyState())
	{
	case EPressedKey::EPK_Q:
		PlayAbilityMontage(Ability_Q_IntroMontage);
		break;
	case EPressedKey::EPK_W:
		PlayAbilityMontage(Ability_W_IntroMontage);
		break;
	case EPressedKey::EPK_E:
		PlayAbilityMontage(Ability_E_IntroMontage);
		break;
	}
}

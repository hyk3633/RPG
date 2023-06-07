#pragma once

UENUM(BlueprintType)
enum class ENotifyCode : uint8
{
	/** ¼Ò¼­·¯ Enum */
	ENC_S_Q_FireRestrictionBall UMETA(DisplayName = "S_R_FireRestrictionBall"),
	ENC_S_W_MeteorliteFall UMETA(DisplayName = "S_W_MeteorliteFall"),
	ENC_S_E_MeteorShower UMETA(DisplayName = "S_E_MeteorShower"),
	ENC_S_R_FloatACharacter UMETA(DisplayName = "S_R_FloatACharacter"),
	ENC_S_R_BlackholeOn UMETA(DisplayName = "S_R_BlackholeOn"),
	ENC_S_R_BlackholeOff UMETA(DisplayName = "S_R_BlackholeOff"),

	ENC_MAX UMETA(DisplayName = "NotifyCode_MAX")
};
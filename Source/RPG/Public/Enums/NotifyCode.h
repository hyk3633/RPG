#pragma once

UENUM(BlueprintType)
enum class ENotifyCode : uint8
{
	/** 소서러 Enum */
	ENC_S_Q_FireRestrictionBall UMETA(DisplayName = "S_R_FireRestrictionBall"),
	ENC_S_W_MeteorliteFall UMETA(DisplayName = "S_W_MeteorliteFall"),
	ENC_S_E_MeteorShower UMETA(DisplayName = "S_E_MeteorShower"),
	ENC_S_R_FloatingCharacter UMETA(DisplayName = "S_R_FloatingCharacter"),
	ENC_S_R_SummonBlackhole UMETA(DisplayName = "S_R_SummonBlackhole"),
	ENC_S_R_BlackholeEnd UMETA(DisplayName = "S_R_BlackholeEnd"),
	ENC_S_R_BlackholeBeam UMETA(DisplayName = "S_R_BlackholeBeam"),

	/** 워리어 Enum */
	ENC_W_Q_Wield UMETA(DisplayName = "W_Q_Wield"),
	ENC_W_W_RevealEnemies UMETA(DisplayName = "W_W_RevealEnemies"),
	ENC_W_E_SmashDown UMETA(DisplayName = "W_E_SmashDown"),
	ENC_W_R_MoveToTargettedLocation UMETA(DisplayName = "W_R_MoveToTargettedLocation"),
	ENC_W_R_Rebirth UMETA(DisplayName = "W_R_Rebirth"),

	ENC_MAX UMETA(DisplayName = "NotifyCode_MAX")
};
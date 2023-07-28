#pragma once

UENUM()
enum class EEnemyAttackType : uint8
{
	EEAT_Melee,
	EEAT_Ranged,
	EEAT_Hybrid,
	EEAT_Boss,

	EEAT_Max

};
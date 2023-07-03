#pragma once

UENUM()
enum class EItemType : uint8
{
	EIT_Coin,
	EIT_HealthPotion,
	EIT_ManaPotion,
	EIT_Weapon,
	EIT_Armour,

	EIT_MAX
};
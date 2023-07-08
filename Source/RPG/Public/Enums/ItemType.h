#pragma once

UENUM()
enum class EItemType : uint8
{
	EIT_Coin,
	EIT_HealthPotion,
	EIT_ManaPotion,
	EIT_Armour,
	EIT_Accessories,

	EIT_MAX
};
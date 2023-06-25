#pragma once

UENUM(BlueprintType)
enum class EPressedKey : uint8
{
	EPK_Q UMETA(DisplayName = "Ability_Q"),
	EPK_W UMETA(DisplayName = "Ability_W"),
	EPK_E UMETA(DisplayName = "Ability_E"),
	EPK_R UMETA(DisplayName = "Ability_R"),
	EPK_None UMETA(DisplayName = "Ability_None"),

	EPK_MAX UMETA(DisplayName = "DefaultMAX")
};
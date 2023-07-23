#pragma once

#include "StatInfo.generated.h"

USTRUCT(Atomic)
struct FStatInfo
{
	GENERATED_BODY()

public:

	UPROPERTY()
	float DefenseivePower;

	UPROPERTY()
	float Dexterity;

	UPROPERTY()
	int32 MaxMP;

	UPROPERTY()
	int32 MaxHP;

	UPROPERTY()
	float StrikingPower;

	UPROPERTY()
	float SkillPower;

	UPROPERTY()
	float AttackSpeed;

	FStatInfo() : DefenseivePower(0), Dexterity(1), MaxMP(0), MaxHP(0), StrikingPower(0), SkillPower(0), AttackSpeed(1) {}
	FStatInfo& operator=(const FStatInfo& Other)
	{
		if (this == &Other) return *this;
		DefenseivePower = Other.DefenseivePower;
		Dexterity = Other.Dexterity;
		MaxMP = Other.MaxMP;
		MaxHP = Other.MaxHP;
		StrikingPower = Other.StrikingPower;
		SkillPower = Other.SkillPower;
		AttackSpeed = Other.AttackSpeed;
		return *this;
	}
};
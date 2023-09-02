#pragma once

#include "Engine/DataTable.h"
#include "EnemyInfo.generated.h"

USTRUCT(BlueprintType)
struct FEnemyInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	FEnemyInfo() : Name(""), StrikingPower(1.f), DefensivePower(1.f), MaxHealth(100), DetectDistance(500), AttackDistance(100) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float StrikingPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float DefensivePower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 DetectDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 AttackDistance;

	FEnemyInfo& operator=(const FEnemyInfo& Other)
	{
		Name = Other.Name;
		StrikingPower = Other.StrikingPower;
		DefensivePower = Other.DefensivePower;
		MaxHealth = Other.MaxHealth;
		DetectDistance = Other.DetectDistance;
		AttackDistance = Other.AttackDistance;

		return *this;
	}
};
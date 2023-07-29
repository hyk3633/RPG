#pragma once

#include "Engine/DataTable.h"
#include "DamageType/DamageTypeBase.h"
#include "ProjectileInfo.generated.h"

USTRUCT(BlueprintType)
struct FProjectileInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FProjectileInfo() : bIsPlayers(false), ExpireTime(1.f), InitialSpeed(5000), CollisionRadius(24), bIsExplosive(false), ExplosionRadius(100), bIsHoming(false), DamageType(UDamageTypeBase::StaticClass()) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bIsPlayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float ExpireTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 CollisionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bIsExplosive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int32 ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	bool bIsHoming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	TSubclassOf<UDamageType> DamageType;

	FProjectileInfo& operator=(const FProjectileInfo& Other)
	{
		bIsPlayers = Other.bIsPlayers;
		bIsExplosive = Other.bIsExplosive;
		ExpireTime = Other.ExpireTime;
		InitialSpeed = Other.InitialSpeed;
		ExplosionRadius = Other.ExplosionRadius;
		bIsHoming = Other.bIsHoming;
		DamageType = Other.DamageType;

		return *this;
	}
};
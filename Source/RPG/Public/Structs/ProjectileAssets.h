#pragma once

#include "Engine/DataTable.h"
#include "ProjectileAssets.generated.h"

USTRUCT(BlueprintType)
struct FProjectileAssets : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FProjectileAssets() : BodyMesh(nullptr), BodyParticle(nullptr), WorldImpactParticle(nullptr), CharacterImpactParticle(nullptr), NoImpactParticle(nullptr) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UStaticMesh* BodyMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UParticleSystem* BodyParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UParticleSystem* WorldImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UParticleSystem* CharacterImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UParticleSystem* NoImpactParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	class USoundCue* ImpactSound;

	FProjectileAssets& operator=(const FProjectileAssets& Other)
	{
		BodyMesh = Other.BodyMesh;
		BodyParticle = Other.BodyParticle;
		WorldImpactParticle = Other.WorldImpactParticle;
		CharacterImpactParticle = Other.CharacterImpactParticle;
		NoImpactParticle = Other.NoImpactParticle;
		ImpactSound = Other.ImpactSound;

		return *this;
	}
};
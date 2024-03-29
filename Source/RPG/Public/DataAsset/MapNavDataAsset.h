
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Structs/Pos.h"
#include "MapNavDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UMapNavDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(VisibleAnywhere)
	FVector NavOrigin;

	UPROPERTY(VisibleAnywhere)
	TArray<FPos> FieldLocations;
	
	UPROPERTY(VisibleAnywhere)
	TArray<float> FieldHeights;

	UPROPERTY(VisibleAnywhere)
	TArray<bool> IsMovableArr;

	UPROPERTY(VisibleAnywhere)
	TArray<int16> ExtraCost;

	UPROPERTY(VisibleAnywhere)
	FString MapName;

	UPROPERTY(VisibleAnywhere)
	int32 GridDist;

	UPROPERTY(VisibleAnywhere)
	int32 GridWidthSize;

	UPROPERTY(VisibleAnywhere)
	int32 GridLengthSize;

	UPROPERTY(VisibleAnywhere)
	float BiasX;

	UPROPERTY(VisibleAnywhere)
	float BiasY;
};

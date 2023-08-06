
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/Pos.h"
#include "WorldGridManagerComponent.generated.h"

class UMapNavDataAsset;

USTRUCT()
struct FAStarNode
{
	GENERATED_BODY()

	int32 F;
	int32 G;
	FPos Pos;

	FAStarNode(int32 _F, int32 _G, FPos _Pos)
		: F(_F), G(_G), Pos(_Pos) {}
	FAStarNode() {}
	bool operator<(const FAStarNode& Other) const { return F < Other.F; }
	bool operator>(const FAStarNode& Other) const { return F > Other.F; }
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UWorldGridManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UWorldGridManagerComponent();

	FORCEINLINE bool GetIsNavigationEnable() const { return bMapNavDataUsable; }

protected:
	
	virtual void BeginPlay() override;

public:	
	
	void InitWorldGrid();

	void AStar(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest);

protected:

	int32 VectorToCoordinatesY(const double& VectorComponent);

	int32 VectorToCoordinatesX(const double& VectorComponent);

	int32 CoordinatesToVectorY(const int32 Coordinates);

	int32 CoordinatesToVectorX(const int32 Coordinates);

	bool CanGo(const FPos& _Pos);

private:

	FPos Front[8] =
	{
		FPos { -1, 0},
		FPos { 0, -1},
		FPos { 1, 0},
		FPos { 0, 1},
		FPos {-1, -1},
		FPos {1, -1},
		FPos {1, 1},
		FPos {-1, 1},
	};
	int32 Cost[8] = { 10,10,10,10,14,14,14,14 };

	FVector NavOrigin;

	int32 GridDist = 0;
	int32 GridWidthSize = 0;
	int32 GridLengthSize = 0;
	int32 TotalSize = 0;

	float WorldOffsetX;
	float WorldOffsetY;

	TArray<FPos> FieldLocations;

	TArray<bool> IsMovableArr;
		
	UPROPERTY()
	UMapNavDataAsset* MapNavDataAsset;

	UPROPERTY(EditAnywhere, Category = "MapNavData")
	FString MapNavDataReference;

	bool bMapNavDataUsable = false;
};

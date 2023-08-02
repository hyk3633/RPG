
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/Pos.h"
#include "WorldGridManagerComponent.generated.h"

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

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitWorldGrid(int32 Number, int32 Interval);

	void AStar(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest);

protected:

	int32 VectorToCoordinates(const double& VectorComponent);

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

	int32 GridSize;

	int32 GridDist;

	float WorldOffset;

	TArray<FPos> _path;

	int32 idx = 0;
		
};

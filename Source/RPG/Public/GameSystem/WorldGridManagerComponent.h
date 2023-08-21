
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Structs/Pos.h"
#include "WorldGridManagerComponent.generated.h"

class UMapNavDataAsset;
class ARPGBaseEnemyCharacter;
class AEnemySpawner;

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

	virtual void InitializeComponent() override;
	
	virtual void BeginPlay() override;

	void DrawGrid();

	void InitWorldGrid();

public:	
	
	void UpdateCharacterExtraCost(int32& CoordinateY, int32& CoordinateX, const FVector& Location);

protected:

	void UpdateCharacterExtraCostValue(const int32& CoordinateY, const int32& CoordinateX, const int8 Value);

public:

	void AStarPlayer(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest);

	int32 VectorToCoordinatesY(const double& VectorComponent);

	int32 VectorToCoordinatesX(const double& VectorComponent);

	int32 CoordinatesToVectorY(const int32 Coordinates);

	int32 CoordinatesToVectorX(const int32 Coordinates);

	bool CanGo(const FPos& _Pos);

private:

	// 이동 가능 방향
	FPos Front[8] =
	{
		FPos { -1, 0},
		FPos { 0, 1},
		FPos { 1, 0},
		FPos { 0, -1},
		FPos {-1, -1},
		FPos {-1, 1},
		FPos {1, 1},
		FPos {1, -1},
	};

	// 방향 별 가중치
	int8 Cost[8] = { 10,10,10,10,14,14,14,14 };

	// 내비게이션 중심 위치
	FVector NavOrigin;

	// 그리드 포인트 간 거리
	int32 GridDist = 0;

	// 그리드 가로,세로 길이
	int32 GridWidthSize = 0;
	int32 GridLengthSize = 0;

	// 그리드 - 벡터 변환 바이어스
	int32 BiasX;
	int32 BiasY;

	// 그리드 포인트 배열
	TArray<FPos> FieldLocations;

	// 해당 인덱스 그리드의 장애물 여부
	TArray<bool> IsMovableArr;

	// 장애물 회피를 위한 추가 가중치
	TArray<int8> ObstacleExtraCost;

	// 캐릭터 위치의 추가 가중치
	TArray<int8> CharacterExtraCost;

	UPROPERTY()
	UMapNavDataAsset* MapNavDataAsset;

	UPROPERTY(EditAnywhere, Category = "MapNavData")
	FString MapNavDataReference;

	bool bMapNavDataUsable = false;
};

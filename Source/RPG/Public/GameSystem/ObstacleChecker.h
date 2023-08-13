
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Structs/Pos.h"
#include "ObstacleChecker.generated.h"

class UBoxComponent;

UCLASS()
class RPG_API AObstacleChecker : public AActor
{
	GENERATED_BODY()
	
public:	

	AObstacleChecker();

protected:

	virtual void BeginPlay() override;

	bool CheckAssetValidity();

	void InitMapSpecification();

	void InitFieldLocations();

	void DrawGridPointInBox();

public:	

	virtual void Tick(float DeltaTime) override;

protected:

	void CheckObstacleSequentially(float DeltaTime);

	void CheckObstacle();

	void GiveExtraScoreToGrid(float DeltaTime);

	void BFS(int32 GridIdx);

	void CreateMapNavDataAsset();

private:

	UPROPERTY(EditInstanceOnly)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditInstanceOnly)
	UBillboardComponent* BillBoard;

	FVector Origin;
	FVector Extent;

	UPROPERTY(EditInstanceOnly, Category = "Nav Setting", meta = (ClampMin = "5", ClampMax = "50"))
	int32 GridDist = 25;

	int32 GridWidthSize = 0;
	int32 GridLengthSize = 0;
	int32 TotalSize = 0;
	int32 BlockedSize = 0;

	int32 BiasX = 0;
	int32 BiasY = 0;

	float CumulatedTime = 0.1f;

	TArray<FPos> FieldLocations;

	TArray<bool> IsMovableArr;

	TArray<int32> BlockedGrids;

	TArray<int8> ExtraCost;

	int32 LastIdx = 0;

	FString AssetPath;

	UPROPERTY(EditInstanceOnly, Category = "Nav Setting")
	FString AssetName;

	UPROPERTY(EditInstanceOnly, Category = "Nav Setting")
	bool bAllowObstacleCheck = false;

	bool bStartCheck = false;

	bool bStartGiveScore = false;

	int8 ObstacleCost = 12;

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
};

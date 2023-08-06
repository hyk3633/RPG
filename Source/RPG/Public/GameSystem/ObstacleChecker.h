
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

	float WorldOffsetX;
	float WorldOffsetY;

	float CumulatedTime = 0.1f;

	TArray<FPos> FieldLocations;

	TArray<bool> IsMovableArr;

	int32 LastIdx = 0;

	FString AssetPath;

	UPROPERTY(EditInstanceOnly, Category = "Nav Setting")
	FString AssetName;

	UPROPERTY(EditInstanceOnly, Category = "Nav Setting")
	bool bAllowObstacleCheck = false;

	bool bStartCheck = false;
};

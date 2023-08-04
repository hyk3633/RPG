
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

	void InitMapSpecification();

	bool LoadMapData();

	void InitFieldLocations();

	void DrawGridPointInBox();

public:	

	virtual void Tick(float DeltaTime) override;

protected:

	void CheckObstacleSequentially(float DeltaTime);

	void CheckObstacle();

	void WriteToFile(const FString& FilePath, const FString& Content);

	bool ReadFile(const FString& FilePath, FString& OutContent);

private:

	UPROPERTY(EditInstanceOnly)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditInstanceOnly)
	UBillboardComponent* BillBoard;

	FVector Origin;
	FVector Extent;
	FVector Start;
	int32 GridDist = 0;

	int32 GridWidthSize = 0;
	int32 GridLengthSize = 0;
	int32 TotalSize = 0;

	float WorldOffsetX;
	float WorldOffsetY;

	float CumulatedTime = 0.1f;

	TArray<FPos> FieldLocations;

	TArray<bool> ObstaclesOrNot;
	FString ObstacleCheckData = "";

	int32 LastIdx = 0;

	UPROPERTY(EditInstanceOnly)
	FString FileName;

	UPROPERTY(EditInstanceOnly)
	bool bAllowObstacleCheck = false;

	bool bStartCheck = false;
};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class ARPGBaseEnemyCharacter;
class UWorldGridManagerComponent;
class AEnemyPooler;
class UBoxComponent;

USTRUCT()
struct FEnemyAndDistance
{
	GENERATED_BODY()

public:

	UPROPERTY()
	ARPGBaseEnemyCharacter* Enemy;

	UPROPERTY()
	float Distance;

	FEnemyAndDistance() : Enemy(nullptr), Distance(0) {}
	FEnemyAndDistance(ARPGBaseEnemyCharacter* _Enemy, float _Distance) : Enemy(_Enemy), Distance(_Distance) {}
	bool operator<(const FEnemyAndDistance& Other) const
	{
		return Distance < Other.Distance;
	}
};

USTRUCT()
struct FEnemiesOfTarget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FEnemyAndDistance> Enemies;

	FEnemiesOfTarget() {}
};

UCLASS()
class RPG_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	

	AEnemySpawner();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAreaBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAreaBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	virtual void Tick(float DeltaTime) override;

protected:

	void CalculateDistanceBetweenPlayersAndEnemies();

	void GetEnemiesPathToPlayers();

	UFUNCTION()
	void EnemyRespawnDelay();

	void EnemyRespawn();

private:

	UPROPERTY(EditAnywhere)
	UBoxComponent* AreaBox;

	UPROPERTY(EditAnywhere)
	UWorldGridManagerComponent* WorldGridManager;

	UPROPERTY()
	TMap<int32, AEnemyPooler*> EnemyPoolerMap;

	FTimerHandle EnemyRespawnTimer;

	UPROPERTY()
	TArray<ACharacter*> PlayersInArea;

	UPROPERTY()
	TArray<ARPGBaseEnemyCharacter*> EnemiesInArea;

	UPROPERTY()
	TMap<ACharacter*, FEnemiesOfTarget> TargetAndEnemies;

	FTimerHandle CalculateDistanceTimer;

	FTimerHandle GettingPathTimer;

	int8 PathOrderIdx = 0;
};


#pragma once

#include "CoreMinimal.h"
#include "Enums/EnemyType.h"
#include "GameFramework/Actor.h"
#include "EnemyPooler.generated.h"

class ARPGBaseEnemyCharacter;
class URPGEnemyFormComponent;

UCLASS()
class RPG_API AEnemyPooler : public AActor
{
	GENERATED_BODY()
	
public:	

	AEnemyPooler();

	void CreatePool(const int32 Size, const EEnemyType Type, const FVector& WaitingLocation);

	void DestroyPool();

	ARPGBaseEnemyCharacter* GetPooledEnemy();

	FORCEINLINE const TArray<ARPGBaseEnemyCharacter*>& GetEnemyArr() const { return EnemyArr; }

protected:

	UFUNCTION()
	void AddDeactivatedNum();

private:

	UPROPERTY()
	URPGEnemyFormComponent* EnemyFormComponent;

	UPROPERTY()
	TSubclassOf<ARPGBaseEnemyCharacter> EnemyCharacterClass;

	UPROPERTY()
	TArray<ARPGBaseEnemyCharacter*> EnemyArr;

	int32 PoolSize;

	int32 ActivatedNum;

	int32 DeactivatedNum;

};

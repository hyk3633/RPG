

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/EnemyType.h"
#include "Enums/EnemyAttackType.h"
#include "Structs/EnemyInfo.h"
#include "Structs/EnemyAssets.h"
#include "RPGEnemyFormComponent.generated.h"

class ARPGBaseEnemyCharacter;
class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class ARPGBaseProjectile;
class UProjectilePoolerComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API URPGEnemyFormComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	URPGEnemyFormComponent();

	void InitEnemyFormComponent(EEnemyType Type);

protected:
	
	void GetEnemyInfoAndInitialize(EEnemyType Type);

	void GetEnemyAssetsAndInitialize(EEnemyType Type);

	void CreateProjectilePooler();

public:	

	ARPGBaseEnemyCharacter* CreateNewEnemy();

protected:

	void InitEnemy(ARPGBaseEnemyCharacter* SpawnedEnemy);

public:
	
	FORCEINLINE const FEnemyInfo& GetEnemyInfo() const { return EnemyInfo; }

	FORCEINLINE const FEnemyAssets& GetEnemyAssets() const { return EnemyAssets; }

	void MeleeAttack(ARPGBaseEnemyCharacter* Attacker);

	void RangedAttack(ARPGBaseEnemyCharacter* Attacker);

protected:

	void GetSocketLocationAndSpawn(ARPGBaseEnemyCharacter* Attacker);

	void SpawnProjectile(ARPGBaseEnemyCharacter* Attacker, const FVector& SpawnLocation, const FRotator& SpawnRotation);
	
private:

	FEnemyInfo EnemyInfo;

	FEnemyAssets EnemyAssets;

	bool bIsWeaponed = false;

	UPROPERTY()
	UProjectilePoolerComponent* ProjectilePooler;
};

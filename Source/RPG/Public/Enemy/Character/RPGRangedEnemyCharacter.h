
#pragma once

#include "CoreMinimal.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "RPGRangedEnemyCharacter.generated.h"

/**
 * 
 */

class ARPGBaseProjectile;

UCLASS()
class RPG_API ARPGRangedEnemyCharacter : public ARPGBaseEnemyCharacter
{
	GENERATED_BODY()

public:

	ARPGRangedEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void InitAnimInstance() override;

public:

	void BTTask_RangedAttack();

	bool CheckCanFireToTarget();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void PlayRangedAttackEffectMulticast();

	UFUNCTION()
	void RangedAttack();

	virtual bool ShouldIStopMovement() override;

private:


};

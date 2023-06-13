
#pragma once

#include "CoreMinimal.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "RPGMeleeEnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGMeleeEnemyCharacter : public ARPGBaseEnemyCharacter
{
	GENERATED_BODY()
	

public:

	ARPGMeleeEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Attack() override;

private:


};

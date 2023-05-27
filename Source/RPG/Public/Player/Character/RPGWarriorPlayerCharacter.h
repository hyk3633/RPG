
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "RPGWarriorPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGWarriorPlayerCharacter : public ARPGBasePlayerCharacter
{
	GENERATED_BODY()

public:

	ARPGWarriorPlayerCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	virtual void Ability_Q() override;
							 
	virtual void Ability_W() override;
							 
	virtual void Ability_E() override;
							 
	virtual void Ability_R() override;

protected:

	UFUNCTION()
	void FindEnemiesInFront();

	bool IsActorInRange(const AActor* Target);

	
};

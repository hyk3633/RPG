
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

protected:

	UFUNCTION()
	void FindEnemiesInFront();

	bool IsActorInRange(const AActor* Target);

	UFUNCTION()
	void RevealNearbyEnemies();
	
	UFUNCTION()
	void PushawayEnemies();
};


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

	/** Q 스킬 함수 */
	UFUNCTION()
	void FindEnemiesInFrontAndDamage();

	bool IsActorInRange(const AActor* Target);

	/** W 스킬 함수 */
	UFUNCTION()
	void RevealNearbyEnemies();
	
	/** E 스킬 함수 */
	UFUNCTION()
	void PushawayNearbyEnemies();

	/** R 스킬 함수 */
	UFUNCTION()
	void AnnihilateNearbyEnemies();

};

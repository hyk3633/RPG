
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

	/** Q ��ų �Լ� */
	UFUNCTION()
	void FindEnemiesInFrontAndDamage();

	bool IsActorInRange(const AActor* Target);

	/** W ��ų �Լ� */
	UFUNCTION()
	void RevealNearbyEnemies();
	
	/** E ��ų �Լ� */
	UFUNCTION()
	void PushawayNearbyEnemies();

	/** R ��ų �Լ� */
	UFUNCTION()
	void AnnihilateNearbyEnemies();

};

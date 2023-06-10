
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

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

protected:

	/** Q ��ų �Լ� */

	UFUNCTION()
	void Wield(ENotifyCode NotifyCode);

	bool IsActorInRange(const AActor* Target);

	/** W ��ų �Լ� */
	UFUNCTION()
	void RevealEnemies(ENotifyCode NotifyCode);

	void DeactivateEnforceParticle();
	
	/** E ��ų �Լ� */

	UFUNCTION()
	void SmashDown(ENotifyCode NotifyCode);

	/** R ��ų �Լ� */
	UFUNCTION()
	void Rebirth(ENotifyCode NotifyCode);

	UFUNCTION(Server,Reliable)
	void RebirthServer();

	UFUNCTION(NetMulticast, Reliable)
	void RebirthMulticast();

	void RebirthReal();

private:

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldImpactParticle;

	UPROPERTY()
	UParticleSystemComponent* EnforceParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceEndParticle;

	FTimerHandle EnforceParticleTimer;

};

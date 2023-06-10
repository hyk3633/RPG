
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

	/** Q 스킬 함수 */

	UFUNCTION()
	void Wield(ENotifyCode NotifyCode);

	bool IsActorInRange(const AActor* Target);

	/** W 스킬 함수 */
	UFUNCTION()
	void RevealEnemies(ENotifyCode NotifyCode);

	void DeactivateEnforceParticle();
	
	/** E 스킬 함수 */

	UFUNCTION()
	void SmashDown(ENotifyCode NotifyCode);

	/** R 스킬 함수 */
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

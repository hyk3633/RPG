
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "RPGWarriorPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGBaseProjectile;

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

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

	UFUNCTION()
	void OnComponentHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Q ��ų �Լ� */

	UFUNCTION()
	void Wield(ENotifyCode NotifyCode);

	UFUNCTION(Server,Reliable)
	void WieldSphereTraceServer();

	void WieldSphereTrace();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnWieldImpactParticleMulticast(const FVector_NetQuantize& SpawnLocation);

	bool IsActorInRange(const AActor* Target);

	/** W ��ų �Լ� */

	UFUNCTION()
	void RevealEnemies(ENotifyCode NotifyCode);

	void EnemyCustomDepthOn();

	UFUNCTION(Server, Reliable)
	void ActivateEnforceParticleServer();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateEnforceParticleMulticast();

	void DeactivateEnforceParticle();
	
	/** E ��ų �Լ� */

	UFUNCTION()
	void SmashDown(ENotifyCode NotifyCode);

	/** R ��ų �Լ� */
	UFUNCTION()
	void Rebirth(ENotifyCode NotifyCode);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	/** Q ��ų */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldStartParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldImpactParticle;

	TArray<FHitResult> WieldHitResults;

	/** W ��ų */

	UPROPERTY()
	UParticleSystemComponent* EnforceParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceEndParticle;

	FTimerHandle EnforceParticleTimer;

	bool bReflectOn = false;

	/** E ��ų */


	/** R ��ų */

};

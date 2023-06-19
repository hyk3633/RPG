
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "RPGWarriorPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGBaseEnemyCharacter;
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

	virtual void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser) override;

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

	UFUNCTION()
	void OnComponentHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** Q 스킬 함수 */

	UFUNCTION()
	void Wield(ENotifyCode NotifyCode);

	UFUNCTION(Server,Reliable)
	void WieldSphereTraceServer();

	void WieldSphereTrace();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnWieldImpactParticleMulticast(const FVector_NetQuantize& SpawnLocation);

	bool IsActorInRange(const AActor* Target);

	/** W 스킬 함수 */

	UFUNCTION()
	void RevealEnemies(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void FindNearbyEnemiesServer();

	void FindNearbyEnemies();

	UFUNCTION()
	void OnRep_bReflectOn();

	void EnemyCustomDepthOn();

	void DeactivateRevealEnemies();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateEnforceParticleMulticast();

	void DeactivateEnforceParticle();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnReflectImpactParticleMulticast(const FVector_NetQuantize& SpawnLocation);
	
	/** E 스킬 함수 */

	UFUNCTION()
	void SmashDown(ENotifyCode NotifyCode);

	/** R 스킬 함수 */
	UFUNCTION()
	void Rebirth(ENotifyCode NotifyCode);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	/** 일반 공격 */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Normal")
	UParticleSystem* NormalAttackImpactParticle;

	/** Q 스킬 */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldStartParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldImpactParticle;

	TArray<FHitResult> WieldHitResults;

	/** W 스킬 */

	UPROPERTY()
	UParticleSystemComponent* EnforceParticleComp;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | RevealEnemies")
	UParticleSystem* EnforceEndParticle;

	FTimerHandle EnforceParticleTimer;

	FTimerHandle DeactivateRevealEnemiesTimer;

	UPROPERTY(ReplicatedUsing = OnRep_bReflectOn)
	bool bReflectOn = false;

	UPROPERTY(Replicated)
	TArray<ARPGBaseEnemyCharacter*> CDepthOnEnemies;

	/** E 스킬 */


	/** R 스킬 */

};

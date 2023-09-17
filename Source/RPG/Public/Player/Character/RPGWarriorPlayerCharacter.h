
#pragma once

#include "CoreMinimal.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "RPGWarriorPlayerCharacter.generated.h"

/**
 * 
 */

class ARPGBaseEnemyCharacter;
class ARPGBaseProjectile;
class USoundCue;

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

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** ��ų ��� */

	virtual void CastAbilityByKey(EPressedKey KeyType) override;

	virtual void CastAbilityAfterTargeting() override;

	virtual void OnAbilityEnded(EPressedKey KeyType) override;

	/** �Ϲ� ���� */

	virtual void CastNormalAttack() override;

	UFUNCTION(Server, Reliable)
	void NormalAttackLineTraceServer();

	void NormalAttackLineTrace();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnNormalAttackEffectMulticast(const FVector_NetQuantize& SpawnLocation);

	/** Q ��ų �Լ� */

	UFUNCTION()
	void Wield(ENotifyCode NotifyCode);

	UFUNCTION(Server,Reliable)
	void WieldServer();

	void SphereTrace(const FVector& Start, const FVector& End, const float& Radius);

	void ApplyWieldEffectToHittedActors();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnWieldImpactParticleMulticast(const FVector_NetQuantize& SpawnLocation);

	/** W ��ų �Լ� */

	UFUNCTION()
	void RevealEnemies(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void RevealEnemiesServer();

	void ActivateReflect();

	UFUNCTION()
	void OnRep_bReflectOn();

	void EnemyCustomDepthOn();

	void DeactivateRevealEnemies();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateEnforceParticleMulticast();

	void DeactivateEnforceParticle();

	UFUNCTION(NetMulticast, Reliable)
	void PlayReflectMontageAndParticleMulticast(const FVector_NetQuantize& SpawnLocation);
	
	/** E ��ų �Լ� */

	UFUNCTION()
	void Dash(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void DashServer();

	UFUNCTION(NetMulticast, Reliable)
	void DashMulticast(const FVector_NetQuantize& DashDirection);

	UFUNCTION()
	void SmashDown(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void SmashDownServer();

	void SmashDownToEnemies();

	/** R ��ų �Լ� */
	UFUNCTION()
	void MoveToTargettedLocation(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void CharacterMoveToTargettedLocationServer();

	UFUNCTION(NetMulticast, Reliable)
	void CharacterMoveToTargettedLocationMulticast();

	UFUNCTION()
	void Rebirth(ENotifyCode NotifyCode);

	UFUNCTION(Server, Reliable)
	void RebirthServer();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	/** �Ϲ� ���� */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Normal")
	UParticleSystem* NormalAttackImpactParticle;

	TArray<FHitResult> NormalAttackHits;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Normal")
	USoundCue* AttackHitSound;

	/** Q ��ų */

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldStartParticle;

	UPROPERTY(EditAnywhere, Category = "Character | Particle | Wield")
	UParticleSystem* WieldImpactParticle;

	TArray<FHitResult> AbilityHitResults;

	/** W ��ų */

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
	TArray<ARPGBaseEnemyCharacter*> CDepthEnemies;

};

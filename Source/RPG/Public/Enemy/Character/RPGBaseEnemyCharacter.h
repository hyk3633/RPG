
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBaseEnemyCharacter.generated.h"

class ARPGEnemyAIController;
class URPGEnemyAnimInstance;

DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);

UCLASS(Abstract)
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

	FDelegateOnAttackEnd DOnAttackEnd;

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

public:

	void BTTask_Attack();

	void OnRenderCustomDepthEffect() const;
	void OffRenderCustomDepthEffect() const;

	void AnnihilatedByPlayer();

	void EnableSuckedIn();

protected:

	ARPGEnemyAIController* MyController;

	UFUNCTION(Server, Reliable)
	void AttackServer();

	UFUNCTION(NetMulticast, Reliable)
	void AttackMulticast();

	void PlayAttackMontage();

	UFUNCTION()
	virtual void Attack() PURE_VIRTUAL(ARPGBaseEnemyCharacter::Attack, );

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	URPGEnemyAnimInstance* MyAnimInst;

public:

	FORCEINLINE URPGEnemyAnimInstance* GetEnemyAnimInstance() const { return MyAnimInst; }

};

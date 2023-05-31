
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBaseEnemyCharacter.generated.h"

class ARPGEnemyAIController;
class URPGEnemyAnimInstance;

DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);

UCLASS()
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

	FDelegateOnAttackEnd DOnAttackEnd;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

public:

	virtual void BTTask_Attack();

	void OnRenderCustomDepthEffect() const;

	void OffRenderCustomDepthEffect() const;

protected:

	UFUNCTION()
	virtual void AttackLineTrace();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	ARPGEnemyAIController* MyController;

	URPGEnemyAnimInstance* MyAnimInst;

public:

	FORCEINLINE URPGEnemyAnimInstance* GetEnemyAnimInstance() const { return MyAnimInst; }

};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBaseEnemyCharacter.generated.h"

class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class UWidgetComponent;
class URPGEnemyHealthBarWidget;

DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnHealthChanged, float HealthPercentage);

UCLASS(Abstract)
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

	FDelegateOnAttackEnd DOnAttackEnd;
	FOnDeathDelegate DOnDeath;
	FDelegateOnHealthChanged DOnHealthChanged;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	/** ü�� */

	void HealthDecrease(const float& Damage);

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HealthBarVisibilityOff();

	/** ���� */

	void EnemyDeath();

	void DestroySelf();

public: /** ���� */

	void BTTask_Attack();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void AttackMulticast();

	void PlayAttackMontage();

	UFUNCTION()
	virtual void Attack() PURE_VIRTUAL(ARPGBaseEnemyCharacter::Attack, );

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:

	/** ���� ��ȯ �Լ� */

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }

	bool GetIsInAir() const;

public: /** ���� */

	void FalldownToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void FalldownMulticast();

	void Falldown();

public: /** ��� */

	void GetupToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void GetupMulticast();

	void Getup();

public: 

	/** Ŀ���� ���� �� / ���� */

	void OnRenderCustomDepthEffect(int8 StencilValue);

	void OffRenderCustomDepthEffect();
	
	/** ��� */

	void InstanceDeath();

	/** ��Ȧ ��ȣ�ۿ� */

	void EnableSuckedInToAllClients();

	UFUNCTION(NetMulticast, Reliable)
	void EnableSuckedInMulticast();

	void EnableSuckedIn();

	/** �ൿ ����, ���� */

public:

	void StopActionToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void StopActionMulticast();

	void StopAction();

	void ResumeAction();

	UFUNCTION(NetMulticast, Reliable)
	void ResumeActionMulticast();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	UPROPERTY()
	ARPGEnemyAIController* MyController;

private:

	UPROPERTY()
	URPGEnemyAnimInstance* MyAnimInst;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	URPGEnemyHealthBarWidget* ProgressBar;

	FTimerHandle HealthBarTimer;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleInstanceOnly, Category = "Enemy | Status")
	float Health = 300.f;

	float MaxHealth = 300.f;

	FTimerHandle DestroyTimer;

	FTimerHandle RestrictionTimer;
};

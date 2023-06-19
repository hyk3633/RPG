
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

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }
	bool GetIsInAir() const;

	FDelegateOnAttackEnd DOnAttackEnd;
	FOnDeathDelegate DOnDeath;
	FDelegateOnHealthChanged DOnHealthChanged;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:

	void BTTask_Attack();

	void OnRenderCustomDepthEffect();

	void OffRenderCustomDepthEffect();

	void AnnihilatedByPlayer();

	void EnableSuckedIn();

protected:

	UFUNCTION(Server, Reliable)
	void AttackServer();

	UFUNCTION(NetMulticast, Reliable)
	void AttackMulticast();

	void PlayAttackMontage();

	UFUNCTION()
	virtual void Attack() PURE_VIRTUAL(ARPGBaseEnemyCharacter::Attack, );

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void HealthDecreaseServer(const float& Damage);

	void HealthDecrease(const float& Damage);

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HealthBarVisibilityOff();

	UFUNCTION(NetMulticast, Reliable)
	void DeathMulticast();

	void PlayDeathMontage();

	void DestroySelf();

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
};

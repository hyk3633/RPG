
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBaseEnemyCharacter.generated.h"

class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class UWidgetComponent;
class URPGEnemyHealthBarWidget;

DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnHealthChanged, float HealthPercentage);

UCLASS(Abstract)
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	virtual void Tick(float DeltaTime) override;

	FORCEINLINE URPGEnemyAnimInstance* GetEnemyAnimInstance() const { return MyAnimInst; }
	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }

	FDelegateOnAttackEnd DOnAttackEnd;
	FDelegateOnHealthChanged DOnHealthChanged;

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

	void OnHealthChanged();

	void HealthBarVisibilityOff();

private:

	UPROPERTY()
	URPGEnemyAnimInstance* MyAnimInst;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	URPGEnemyHealthBarWidget* ProgressBar;

	FTimerHandle HealthBarTimer;

	float Health = 300.f;

	float MaxHealth = 300.f;
};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EnemyType.h"
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

	void InitEnemyData();

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	float CalculateDamage(const float& Damage);

	/** 체력 */

	void HealthDecrease(const int32& Damage);

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HealthBarVisibilityOff();

	/** 죽음 */

	void EnemyDeath();

	void DestroySelf();

public: /** 공격 */

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

	/** 설정 반환 함수 */

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }

	bool GetIsInAir() const;

public: /** 기절 */

	void FalldownToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void FalldownMulticast();

	void Falldown();

public: /** 기상 */

	void GetupToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void GetupMulticast();

	void Getup();

public: 

	/** 커스텀 뎁스 온 / 오프 */

	void OnRenderCustomDepthEffect(int8 StencilValue);

	void OffRenderCustomDepthEffect();
	
	/** 즉사 */

	void InstanceDeath();

	/** 블랙홀 상호작용 */

	void EnableSuckedInToAllClients();

	UFUNCTION(NetMulticast, Reliable)
	void EnableSuckedInMulticast();

	void EnableSuckedIn();

	/** 행동 정지, 해제 */

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

	EEnemyType EnemyType;

private:

	UPROPERTY()
	URPGEnemyAnimInstance* MyAnimInst;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	URPGEnemyHealthBarWidget* ProgressBar;

	FTimerHandle HealthBarTimer;

	/** 캐릭터 스탯 */

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleInstanceOnly, Category = "Enemy | Status")
	float Health;

	UPROPERTY(Replicated)
	float MaxHealth;

	UPROPERTY(Replicated)
	FString Name;

	float StrikingPower;

	float DefensivePower;

	int32 Exp;

	FTimerHandle DestroyTimer;

	FTimerHandle RestrictionTimer;
};

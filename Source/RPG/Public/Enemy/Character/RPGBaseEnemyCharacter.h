
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EnemyAttackType.h"
#include "Structs/EnemyAssets.h"
#include "RPGBaseEnemyCharacter.generated.h"

class URPGEnemyFormComponent;
class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class UWidgetComponent;
class URPGEnemyHealthBarWidget;

DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_MULTICAST_DELEGATE(FOnActivateDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnHealthChanged, float HealthPercentage);

UCLASS()
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	void SetEnemyAssets(const FEnemyAssets& NewEnemyAssets);

	void ActivateEnemy();

protected:

	void SetMeshAndController();

	UFUNCTION()
	void OnRep_EnemyAssets();
	
	virtual void InitAnimInstance();

	UFUNCTION()
	void OnRep_bIsActivated();

	void SetCollisionActivate();

public:

	virtual void Tick(float DeltaTime) override;

	FDelegateOnAttackEnd DOnAttackEnd;
	FOnDeathDelegate DOnDeath;
	FDelegateOnHealthChanged DOnHealthChanged;
	FOnActivateDelegate DOnActivate;

	friend URPGEnemyFormComponent;

	void AttachWeaponStaticMesh(UStaticMesh* NewMesh, FName SocketName);

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	/** 체력 */

	virtual void HealthDecrease(const int32& Damage);

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HideMesh();

	void SetCollisionDeactivate();

	void HealthBarVisibilityOff();

	/** 죽음 */

	void EnemyDeath();

public: /** 공격 */

	virtual void BTTask_Attack();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void PlayMeleeAttackEffectMulticast();

	virtual void PlayMeleeAttackEffect();

	UFUNCTION()
	virtual void Attack();

	UFUNCTION()
	virtual void OnAttackMontageEnded();

public:

	/** 설정 반환 함수 */

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }
	FORCEINLINE URPGEnemyAnimInstance* GetAnimInstance() const { return MyAnimInst; }
	FORCEINLINE bool GetIsActivated() const { return bIsActivated; }
	bool GetSuckedIn() const;

	APawn* GetTarget() const;

	bool GetIsInAir() const;

protected:

	/** 기절 상태 및 기절 상태 회복 */

	UFUNCTION(NetMulticast, Reliable)
	void FalldownMulticast();

	void Falldown();

	UFUNCTION(NetMulticast, Reliable)
	void GetupMulticast();

	void Getup();

public: 

	/** 커스텀 뎁스 온 / 오프 */

	void OnRenderCustomDepthEffect(int8 StencilValue);

	void OffRenderCustomDepthEffect();

	/** 블랙홀 상호작용 */

	void EnableSuckedInToAllClients();

	void DisableSuckedInToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void EnableSuckedInMulticast();

	void EnableSuckedIn();

	UFUNCTION(NetMulticast, Reliable)
	void DisableSuckedInMulticast();

	void DisableSuckedIn();

	/** 경직 상태 및 해제 */

	UFUNCTION(NetMulticast, Reliable)
	void StopActionMulticast();

	void StopAction();

	void ResumeAction();

	UFUNCTION(NetMulticast, Reliable)
	void ResumeActionMulticast();

	void SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	UPROPERTY()
	ARPGEnemyAIController* MyController;

	UPROPERTY()
	URPGEnemyFormComponent* EnemyForm;

	UPROPERTY()
	URPGEnemyAnimInstance* MyAnimInst;

	EEnemyAttackType AttackType;

private:

	UPROPERTY()
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	URPGEnemyHealthBarWidget* ProgressBar;

	UPROPERTY(ReplicatedUsing = OnRep_EnemyAssets)
	FEnemyAssets EnemyAssets;

	FTimerHandle HealthBarTimer;

	FTimerHandle RestrictionTimer;

	FTimerHandle HideMeshTimer;

	FTimerHandle FalldownTimer;

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

	UPROPERTY(ReplicatedUsing = OnRep_bIsActivated)
	bool bIsActivated = false;
};

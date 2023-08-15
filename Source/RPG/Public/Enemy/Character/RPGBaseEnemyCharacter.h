
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EnemyAttackType.h"
#include "Structs/EnemyAssets.h"
#include "Structs/Pos.h"
#include "RPGBaseEnemyCharacter.generated.h"

class URPGEnemyFormComponent;
class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class UWidgetComponent;
class URPGEnemyHealthBarWidget;

DECLARE_MULTICAST_DELEGATE(FDelegateMoveEnd);
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

	FDelegateMoveEnd DMoveEnd;
	FDelegateOnAttackEnd DOnAttackEnd;
	FOnDeathDelegate DOnDeath;
	FDelegateOnHealthChanged DOnHealthChanged;
	FOnActivateDelegate DOnActivate;

	friend URPGEnemyFormComponent;

protected:

	virtual void PostInitializeComponents() override;

	void AttachWeaponStaticMesh(UStaticMesh* NewMesh, FName SocketName);

	virtual void BeginPlay() override;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void SetMatInstDynamicMulticast(float LastDamagedTime);

	/** ü�� */

	virtual void HealthDecrease(const int32& Damage);

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HideMesh();

	void SetCollisionDeactivate();

	void HealthBarVisibilityOff();

	/** ���� */

	void EnemyDeath();

public: 

	/** �̵� */

	void BTTask_Move();

protected:

	void UpdateMovement();

public:
	
	/** ���� */

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

	/** ���� ��ȯ �Լ� */

	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }
	FORCEINLINE URPGEnemyAnimInstance* GetAnimInstance() const { return MyAnimInst; }
	FORCEINLINE bool GetIsActivated() const { return bIsActivated; }
	FORCEINLINE int32 GetAttackDistance() const { return Distance; }

	bool GetSuckedIn() const;

	APawn* GetTarget() const;

	bool GetIsInAir() const;

protected:

	/** ���� ���� �� ���� ���� ȸ�� */

	UFUNCTION(NetMulticast, Reliable)
	void FalldownMulticast();

	void Falldown();

	UFUNCTION(NetMulticast, Reliable)
	void GetupMulticast();

	void Getup();

public: 

	/** Ŀ���� ���� �� / ���� */

	void OnRenderCustomDepthEffect(int8 StencilValue);

	void OffRenderCustomDepthEffect();

	/** ��Ȧ ��ȣ�ۿ� */

	void EnableSuckedInToAllClients();

	void DisableSuckedInToAllClients();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void EnableSuckedInMulticast();

	void EnableSuckedIn();

	UFUNCTION(NetMulticast, Reliable)
	void DisableSuckedInMulticast();

	void DisableSuckedIn();

	/** ���� ���� �� ���� */

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

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	EEnemyAttackType AttackType;

private:

	UPROPERTY()
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY()
	URPGEnemyHealthBarWidget* ProgressBar;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> MatInsts;

	int8 MatInstsNum;

	UPROPERTY(ReplicatedUsing = OnRep_EnemyAssets)
	FEnemyAssets EnemyAssets;

	FTimerHandle HealthBarTimer;

	FTimerHandle RestrictionTimer;

	FTimerHandle HideMeshTimer;

	FTimerHandle FalldownTimer;

	/** ĳ���� ���� */

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleInstanceOnly, Category = "Enemy | Status")
	float Health;

	UPROPERTY(Replicated)
	float MaxHealth;

	UPROPERTY(Replicated)
	FString Name;

	float DefensivePower;

	int32 Exp;

	int32 Distance;

	UPROPERTY(ReplicatedUsing = OnRep_bIsActivated)
	bool bIsActivated = false;

	/** �̵� */

	int32 LastTimeY = -1;
	int32 LastTimeX = -1;

	TArray<FPos> PathArr;

	bool bUpdateMovement = false;

	float CulmulativeTime = 0.1f;

	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;
};

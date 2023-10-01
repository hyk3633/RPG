
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/EnemyAttackType.h"
#include "Enums/EnemyType.h"
#include "Structs/EnemyAssets.h"
#include "Structs/Pos.h"
#include "RPGBaseEnemyCharacter.generated.h"

class ARPGGameModeBase;
class AEnemySpawner;
class URPGEnemyFormComponent;
class ARPGEnemyAIController;
class URPGEnemyAnimInstance;
class UWidgetComponent;
class URPGEnemyHealthBarWidget;
class AEnemySpawner;
class UPaperSpriteComponent;
class USoundCue;

DECLARE_MULTICAST_DELEGATE(FDelegateMoveEnd);
DECLARE_MULTICAST_DELEGATE(FDelegateOnAttackEnd);
DECLARE_MULTICAST_DELEGATE(FOnDeathDelegate);
DECLARE_MULTICAST_DELEGATE(FOnActivateDelegate);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDeactivateDelegate, EEnemyType Type, ARPGBaseEnemyCharacter* Enemy);
DECLARE_MULTICAST_DELEGATE_OneParam(FDelegateOnHealthChanged, float HealthPercentage);

UCLASS()
class RPG_API ARPGBaseEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBaseEnemyCharacter();

	void SetEnemyAssets(const FEnemyAssets& NewEnemyAssets);

	virtual void ActivateEnemy(const FVector& Location);

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
	FOnDeactivateDelegate DOnDeactivate;

	friend URPGEnemyFormComponent;

protected:

	virtual void PostInitializeComponents() override;

	void AttachWeaponStaticMesh(UStaticMesh* NewMesh, FName SocketName);

	virtual void BeginPlay() override;

	UFUNCTION()
	void PlayerOut(ACharacter* TargetPlayer);

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION(NetMulticast, Reliable)
	void SetMatInstDynamicMulticast(float LastDamagedTime);

	/** 체력 */

	virtual void HealthDecrease(const int32& Damage);

public:

	void RespawnDelay();

protected:

	void EnemyRespawn();

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	void HideMesh();

	void SetCollisionDeactivate();

	void HealthBarVisibilityOff();

	/** 죽음 */

	void EnemyDeath();

public: 

	/** 이동 */

	void GetPathToTarget(const FVector& Destination, AEnemySpawner* Spanwer);

	void BTTask_Move();

protected:

	UFUNCTION()
	void OnCapsuleCollisionEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void InitPathStatus();

	void UpdatePathIndexAndGridPassability();

	void ResetCurrentGridPassbility();

	void UpdateMovementFlowField();

	void UpdateMovementAStar();

	void RecalculatingPathToTarget();

	virtual bool ShouldIStopMovement();

	void CheckTargetLocation();

	void StopMovement();

public:
	
	/** 공격 */

	virtual void BTTask_Attack();

protected:

	UFUNCTION(NetMulticast, Reliable)
	void PlayMeleeAttackEffectMulticast();

	virtual void PlayMeleeAttackEffect();

	UFUNCTION()
	virtual void Attack();

	UFUNCTION(NetMulticast, UnReliable)
	void PlaySoundMulticast(const FVector_NetQuantize& Location);

	UFUNCTION()
	virtual void OnAttackMontageEnded();

public:

	/** 설정 반환 함수 */

	void SetSpawner(AEnemySpawner* Spawner);

	bool GetSuckedIn() const;

	APawn* GetTarget() const;

	bool GetIsInAir() const;

	FORCEINLINE URPGEnemyFormComponent* GetFormComponent() const { return EnemyForm; }
	FORCEINLINE AEnemySpawner* GetSpawner() const { return MySpawner; }
	FORCEINLINE void SetAIController(ARPGEnemyAIController* AICont) { MyController = AICont; }
	FORCEINLINE URPGEnemyAnimInstance* GetAnimInstance() const { return MyAnimInst; }
	FORCEINLINE bool GetIsActivated() const { return bIsActivated; }
	FORCEINLINE int32 GetDetectDistance() const { return DetectDistance; }
	FORCEINLINE int32 GetAttackDistance() const { return AttackDistance; }
	FORCEINLINE EEnemyAttackType GetAttackType() const { return AttackType;	}
	FORCEINLINE float GetSpeedAdjustmentValue() const { return SpeedAdjustmentValue; }
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }

protected:

	/** 히트 리액션 */

	UFUNCTION(NetMulticast, Reliable)
	void HitReactionMulticast();

	void HitReaction();

	UFUNCTION()
	void OnHitReactionEnded();

	/** 기절 상태 및 기절 상태 회복 */

	UFUNCTION(NetMulticast, Reliable)
	void FalldownMulticast();

	void Falldown();

	UFUNCTION(NetMulticast, Reliable)
	void GetupMulticast();

	void Getup();

	void GetupDelayEnd();

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

	UPROPERTY(EditAnywhere)
	UWidgetComponent* HealthBarWidget;

	EEnemyAttackType AttackType;

private:

	UPROPERTY()
	ARPGGameModeBase* RPGGameMode;

	UPROPERTY()
	AEnemySpawner* MySpawner;

	UPROPERTY()
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* EnemyIconSprite;

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

	FTimerHandle GetupDelayTimer;

	UPROPERTY(ReplicatedUsing = OnRep_bIsActivated)
	bool bIsActivated = false;

	bool bIsAttacking = false;

	EEnemyType EnemyType;

	FDelegateHandle DHandle;

	/** 캐릭터 스탯 */

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleInstanceOnly, Category = "Enemy | Status")
	float Health;

	UPROPERTY(Replicated)
	float MaxHealth;

	UPROPERTY(Replicated)
	FString Name;

	float DefensivePower;

	int32 Exp;

	int32 DetectDistance;

	int32 AttackDistance;

	/** 이동 */

	int32 LastTimeY = -1;
	int32 LastTimeX = -1;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Movement")
	bool bUpdateMovement = false;

	FTimerHandle RespawnTimer;

	float DefaultSpeed = 20.f;

	UPROPERTY(VisibleInstanceOnly, Category = "Enemy | Movement")
	float SpeedAdjustmentValue = 1.f;

	FVector OriginLocation;

	// AStar 방식에서만 사용
	TArray<FPos> PathArr;
	TArray<int32> PathCost;

	FVector NextPoint;
	FVector	NextDirection;
	int32 CurrentPathIdx;

	FVector TargetLocation;

	FTimerHandle CheckTargetLocationTimer;

	const float Seperation_Distance = 50.f;
	int8 Count_Sep;

	float ctime = 0.f;
};

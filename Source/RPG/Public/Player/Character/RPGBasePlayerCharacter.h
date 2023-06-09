
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/PressedKey.h"
#include "RPGBasePlayerCharacter.generated.h"

class ARPGPlayerController;
class URPGAnimInstance;
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class ARPGBaseEnemyCharacter;
struct FCharacterStats;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeHealthPercentageDelegate, float Percentage);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeManaPercentageDelegate, float Percentage);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityCooldownEndDelegate, int8 Bit);

UCLASS()
class RPG_API ARPGBasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBasePlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	FOnChangeHealthPercentageDelegate DOnChangeHealthPercentage;

	FOnChangeManaPercentageDelegate DOnChangeManaPercentage;

	FOnAbilityCooldownEndDelegate DOnAbilityCooldownEnd;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION()
	void OnTargetingComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTargetingComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** ---------- 죽음 ---------- */

	AController* TempController;

	FTimerHandle RespawnTimer;

	void PlayerRespawn();

	void PlayerDie();

	UFUNCTION()
	void AfterDeath();

	/** ---------- 체력 ---------- */

	UFUNCTION()
	void OnRep_Health();

	void UsingMana(EPressedKey KeyType);

	UFUNCTION()
	void OnRep_Mana();

public:	

	void ResetHealthManaUI();

	void RecoveryHealth(const int32 RecoveryAmount);

	void RecoveryMana(const int32 RecoveryAmount);
	
	/** 카메라 줌 인 아웃 */

	void CameraZoomInOut(int8 Value);

public: /** ---------- 이동 ---------- */

	void StopMove();

	void SetDestinationAndPath();

protected:

	void InitDestAndDir();

	UFUNCTION(Server, Reliable)
	void SetDestinaionAndPathServer(const FVector& HitLocation);

	void UpdateMovement();

	UFUNCTION()
	void OnRep_PathX();

	void SpawnClickParticle(const FVector& EmitLocation);

public: /** ---------- 일반 공격 ---------- */

	void DoNormalAttack();

protected:

	void GetHitCursor();

	UFUNCTION(Server, Reliable)
	void GetHitCursorServer(const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void NormalAttackWithComboServer();

	UFUNCTION(NetMulticast, Reliable)
	void NormalAttackWithComboMulticast();

	void NormalAttackWithCombo();

	void TurnTowardAttackPoint();

	UFUNCTION()
	void OnAttackMontageEnded();

	void AttackEndComboState();

	UFUNCTION()
	virtual void CastNormalAttack();

public:	/** ---------- 스킬 사용 준비 ---------- */

	void ReadyToCastAbilityByKey(EPressedKey KeyType);

protected:

	UFUNCTION(Server, Reliable)
	void CastAbilityByKeyServer(EPressedKey KeyType);

	UFUNCTION(NetMulticast, Reliable)
	void CastAbilityByKeyMulticast(EPressedKey KeyType);

	virtual void CastAbilityByKey(EPressedKey KeyType);

	void DrawTargetingCursor();

	void TargetingCompOn(const float& SphereRadius);

	void TargetingCompOff();

public:	/** ---------- 스킬 사용 취소 ---------- */

	virtual void CancelAbility();

public:	/** ---------- 타게팅 후 스킬 사용 ---------- */

	void GetCursorHitResultCastAbility();

protected:

	UFUNCTION(Server, Reliable)
	void CastAbilityAfterTargetingServer();

	UFUNCTION(NetMulticast, Reliable)
	void CastAbilityAfterTargetingMulticast();

	virtual void CastAbilityAfterTargeting();

	/** ---------- 스킬 사용 제한 ---------- */

	void AbilityActiveBitSet(EPressedKey KeyType);

	UFUNCTION(Client, Reliable)
	void AbilityActiveBitOffClient(EPressedKey KeyType);

	void AbilityActiveBitOff(EPressedKey KeyType);

	UFUNCTION(Server, Reliable)
	void AbilityCooldownStartServer(EPressedKey KeyType);

	void AbilityCooldownStart(EPressedKey KeyType);

	UFUNCTION(Client, Reliable)
	void AbilityCooldownEndClient(int8 Bit);

	UFUNCTION()
	virtual void OnAbilityEnded(EPressedKey KeyType) PURE_VIRTUAL(ARPGBasePlayerCharacter::OnAbilityEnded, );

	void SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLoc, const FRotator& SpawnRot = FRotator::ZeroRotator);

public: /** ---------- 반환 및 설정 함수 ---------- */

	FORCEINLINE URPGAnimInstance* GetRPGAnimInstance() const { return RPGAnimInstance; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE int32 GetCurrentCombo() const { return CurrentCombo; }
	FORCEINLINE int8 GetAbilityCooldownBit() const { return AbilityCooldownBit; }
	float GetCooldownPercentage(int8 Bit) const;
	bool IsAbilityAvailable(EPressedKey KeyType);
	bool GetIsAnyMontagePlaying() const;
	bool GetAbilityERMontagePlaying();
	void SetCharacterArmourStats(const float Def, const float Dex, const int32 ExHP, const int32 ExMP);
	void SetCharacterAccessoriesStats(const float Stk, const float Skp, const float Atks);
	
protected:

	UFUNCTION()
	void OnRep_MaxHP();

	UFUNCTION()
	void OnRep_MaxMP();

	void SetAbilityCooldownTime(int8 QTime, int8 WTime, int8 ETime, int8 RTime);

	void SetAbilityManaUsage(int32 QUsage, int32 WUsage, int32 EUsage, int32 RUsage);
	
	void GetTargetingCompOverlappingEnemies(TArray<AActor*>& Enemies);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	URPGAnimInstance* RPGAnimInstance;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

private:

	UPROPERTY(EditAnywhere)
	USphereComponent* TargetingComp;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	int32 NextArmLength = 1200;

	bool bZooming = false;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Character | Click")
	UParticleSystem* ClickParticle;

	FHitResult CursorHitResult;

	UPROPERTY()
	TArray<ARPGBaseEnemyCharacter*> OutlinedEnemies;

protected:

	bool bAiming = false;

	FHitResult TargetingHitResult;

private:

	/** 스탯 */

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Health, Category = "Character | Status")
	float Health = 200.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxHealth = 200.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Mana, Category = "Character | Status")
	float Mana = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxMana = 100.f;

	TArray<int32> ManaUsage;

	/** 서버 전용 스탯 */

	UPROPERTY()
	float DefenseivePower;

	UPROPERTY()
	float StrikingPower;

	UPROPERTY()
	float SkillPower;

	/** 서버, 클라이언트 스탯 */

	UPROPERTY(ReplicatedUsing = OnRep_MaxHP)
	int32 MaxHP;

	UPROPERTY(Replicated = OnRep_MaxMP)
	int32 MaxMP;

	/** 멀티캐스트 스탯 */

	UPROPERTY(Replicated)
	float Dexterity;

	UPROPERTY(Replicated)
	float AttackSpeed;

	/** 이동 */

	UPROPERTY(ReplicatedUsing = OnRep_PathX)
	TArray<float> PathX;

	UPROPERTY(Replicated)
	TArray<float> PathY;

	bool bUpdateMovement = false;

	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;

	/** 일반 공격 */

protected:

	int32 MaxCombo = 4;

protected:

	bool bIsAttacking = false;
	bool bCanNextCombo = false;
	int32 CurrentCombo = 0;

	/** 쿨 타임 */

	int8 AbilityActiveBit = 0;

	UPROPERTY(Replicated, VisibleAnywhere)
	int8 AbilityCooldownBit = 0;

	UPROPERTY(Replicated)
	TArray<float> RemainedCooldownTime;

	TArray<float> MaxCooldownTime;

};

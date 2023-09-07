
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/PressedKey.h"
#include "Enums/MontageEnded.h"
#include "Structs/StatInfo.h"
#include "Structs/Pos.h"
#include "DamageType/DamageTypeBase.h"
#include "RPGBasePlayerCharacter.generated.h"

class ARPGPlayerController;
class URPGAnimInstance;
class USpringArmComponent;
class UCameraComponent;
class USphereComponent;
class ARPGBaseEnemyCharacter;
class UDamageTypeBase;
class USceneCaptureComponent2D;
class UPaperSpriteComponent;
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
	void OnRep_bStunned();

	UFUNCTION()
	void OnStunMontageEnded(EMontageEnded MontageType);

	UFUNCTION(Server, Reliable)
	void StunEndServer();

	void ApplyDamageToEnemy(APawn* TargetEnemy, const float& Damage, TSubclassOf<UDamageType> DamageType = UDamageTypeBase::StaticClass());

	UFUNCTION()
	void OnTargetingComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTargetingComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void PossessedBy(AController* NewController) override;

	/** ---------- 죽음 ---------- */

	UFUNCTION(NetMulticast, Reliable)
	void SetCharacterDeadStateMulticast();

	void PlayerRespawn();

	void PlayerDie();

	/** ---------- 체력 ---------- */

	UFUNCTION()
	void OnRep_Health();

	void UsingMana(EPressedKey KeyType);

	UFUNCTION()
	void OnRep_Mana();

public:	

	void ResetHealthMana();

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
	void ReplicatebUpdateMovementServer(const bool UpdateMovement);

	UFUNCTION(Server, Reliable)
	void SetDestinaionAndPathServer(const FVector_NetQuantize& HitLocation);

	void UpdateMovement();

	UFUNCTION()
	void OnRep_PathArr();

	void SpawnClickParticle(const FVector& EmitLocation);

public: /** ---------- 일반 공격 ---------- */

	void DoNormalAttack();

protected:

	void GetHitCursor();

	UFUNCTION(Server, Reliable)
	void GetHitCursorServer(const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void NormalAttackWithComboServer();

	void NormalAttackWithCombo();

	UFUNCTION(NetMulticast, Reliable)
	void PlayNormalAttackMontageMulticast(const int32 Combo);

	void TurnTowardAttackPoint();

	UFUNCTION()
	void OnAttackMontageEnded(EMontageEnded MontageType);

	UFUNCTION(Server, Reliable)
	void AttackEndComboStateServer();

	UFUNCTION()
	virtual void CastNormalAttack();

	UFUNCTION(Server, Reliable)
	void CastNormalAttackServer();

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
	FORCEINLINE float GetStrikingPower() const { return CharacterStrikingPower + EquipmentStrikingPower; }
	FORCEINLINE bool GetStunned() const { return bStunned; }
	float GetSkillPower(EPressedKey KeyType);
	float GetCooldownPercentage(int8 Bit) const;
	bool IsAbilityAvailable(EPressedKey KeyType);
	bool IsAnyMontagePlaying() const;
	bool IsNormalAttackMontagePlaying() const;
	bool IsAbilityERMontagePlaying();

	/** ---------- 캐릭터 스탯 설정 ---------- */

	void InitCharacterStats(const FStatInfo& NewStats);

	void SetEquipmentArmourStats(const float& Def, const float& Dex, const int32& MxHP, const int32& MxMP);

	void SetEquipmentAccessoriesStats(const float& Stk, const float& Skp, const float& Atks);
	
protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void SetEquipmentArmourStatsServer(const float& Def, const float& Dex, const int32& MxHP, const int32& MxMP);

	UFUNCTION()
	void OnRep_CharacterDexterity();

	UFUNCTION()
	void OnRep_EquipmentDexterity();

	UFUNCTION(Server, Reliable, WithValidation)
	void SetEquipmentAccessoriesStatsServer(const float& Stk, const float& Skp, const float& Atks);

	UFUNCTION()
	void OnRep_CharacterAttackSpeed();

	UFUNCTION()
	void OnRep_EquipmentAttackSpeed();

public:

	void InitializeEquipmentArmourStats();

	void InitializeEquipmentAccessoriesStats();

protected:

	UFUNCTION(Server, Reliable)
	void InitializeEquipmentArmourStatsServer();

	UFUNCTION(Server, Reliable)
	void InitializeEquipmentAccessoriesStatsServer();

	UFUNCTION()
	void OnRep_CharacterMaxHP();

	UFUNCTION()
	void OnRep_EquipmentMaxHP();

	UFUNCTION()
	void OnRep_CharacterMaxMP();

	UFUNCTION()
	void OnRep_EquipmentMaxMP();

	void SetAbilityCooldownTime(const int8& QTime, const int8& WTime, const int8& ETime, const int8& RTime);

	void SetAbilityManaUsage(const int32& QUsage, const int32& WUsage, const int32& EUsage, const int32& RUsage);

	void SetSkillPowerCorrectionValues(const float& QPower, const float& WPower, const float& EPower, const float& RPower);
	
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

	UPROPERTY(EditAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* MinimapArm;

	UPROPERTY(EditAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* MinimapCapture;

	UPROPERTY(EditAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* PlayerIconSprite;

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
	float Health;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Mana, Category = "Character | Status")
	float Mana = 100.f;

	TArray<int32> ManaUsage;

	/** 서버 전용 스탯 */

	float CharacterDefensivePower;

	float EquipmentDefensivePower;

	float CharacterStrikingPower;

	float EquipmentStrikingPower;

	float CharacterSkillPower;

	float EquipmentSkillPower;

	/** 서버, 클라이언트 스탯 */

	UPROPERTY(ReplicatedUsing = OnRep_CharacterMaxHP)
	int32 CharacterMaxHP;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentMaxHP)
	int32 EquipmentMaxHP;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterMaxMP)
	int32 CharacterMaxMP;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentMaxMP)
	int32 EquipmentMaxMP;

	/** 멀티캐스트 스탯 */

	UPROPERTY(ReplicatedUsing = OnRep_CharacterDexterity)
	float CharacterDexterity;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentDexterity)
	float EquipmentDexterity;

	UPROPERTY(ReplicatedUsing = OnRep_CharacterAttackSpeed)
	float CharacterAttackSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentAttackSpeed)
	float EquipmentAttackSpeed;

	/** 이동 */

	UPROPERTY(ReplicatedUsing = OnRep_PathArr)
	TArray<FPos> PathArr;

	bool bUpdateMovement = false;

	float CulmulativeTime = 0.1f;
	int32 LastTimeY = -1;
	int32 LastTimeX = -1;

	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;

	UPROPERTY(ReplicatedUsing = OnRep_bStunned)
	bool bStunned = false;

protected:

	/** 일반 공격 */

	int32 MaxCombo = 4;

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

	/** 리스폰 */

	AController* TempController;

	FTimerHandle RespawnTimer;

	/** 스킬 공격력 보정 값 */

	TArray<float> SkillPowerCorrectionValues;
};

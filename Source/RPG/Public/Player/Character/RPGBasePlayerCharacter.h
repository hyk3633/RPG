
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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangeHealthPercentageDelegate, float Percentage);

UCLASS()
class RPG_API ARPGBasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBasePlayerCharacter();

	virtual void Tick(float DeltaTime) override;

	FOnChangeHealthPercentageDelegate DOnChangeHealthPercentage;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION()
	void OnTargetingComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTargetingComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	/** 카메라 줌 인 아웃 */

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

	UFUNCTION(NetMulticast, Reliable)
	void GetHitCursorMulticast(const FHitResult& Hit);

	UFUNCTION(Server, Reliable)
	void NormalAttackWithComboServer();

	UFUNCTION(NetMulticast, Reliable)
	void NormalAttackWithComboMulticast();

	void NormalAttackWithCombo();

	void TurnTowardAttackPoint();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

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

	void TargetingCompOn();

	void TargetingCompOff();

public:	/** ---------- 스킬 사용 취소 ---------- */

	virtual void CancelAbility();

protected:

	UFUNCTION(Server, Reliable)
	void CancelAbilityServer();

	UFUNCTION(NetMulticast, Reliable)
	virtual void CancelAbilityMulticast();

public:	/** ---------- 타게팅 후 스킬 사용 ---------- */

	void GetCursorHitResultCastAbility();

protected:

	UFUNCTION(Server, Reliable)
	void CastAbilityAfterTargetingServer();

	UFUNCTION(NetMulticast, Reliable)
	void CastAbilityAfterTargetingMulticast();

	virtual void CastAbilityAfterTargeting();

protected:

	/** ---------- 죽음 ---------- */

	void PlayerDie();

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLoc, const FRotator& SpawnRot = FRotator::ZeroRotator);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:

	FORCEINLINE URPGAnimInstance* GetRPGAnimInstance() const { return RPGAnimInstance; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE int32 GetCurrentCombo() const { return CurrentCombo; }
	bool GetIsMontagePlaying() const;

protected:

	URPGAnimInstance* RPGAnimInstance;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

	bool bAiming = false;

	int32 MaxCombo = 4;

	UPROPERTY(Replicated)
	FHitResult TargetingHitResult;

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
	TArray<ACharacter*> OutlinedEnemies;

	/** 스탯 */

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character | Status")
	float Health = 200.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxHealth = 200.f;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character | Status")
	float Mana = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxMana = 100.f;

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

	bool bIsAttacking = false;
	bool bCanNextCombo = false;
	int32 CurrentCombo = 0;

};


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enums/PressedKey.h"
#include "RPGBasePlayerCharacter.generated.h"

class ARPGPlayerController;
class URPGAnimInstance;
class USpringArmComponent;
class UCameraComponent;

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
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);

public:	

	/** �̵� */

	void StopMove();

	void SetDestinationAndPath();

	/** �Ϲ� ���� */

	void DoNormalAttack();

	UFUNCTION(Client, Reliable)
	void GetHitCursorClient();

	UFUNCTION(Server, Reliable)
	void GetHitCursorServer(FHitResult Hit);

	UFUNCTION(NetMulticast, Reliable)
	void GetHitCursorMulticast(FHitResult Hit);

	/** ��ų ��� �غ� */

	UFUNCTION(Server, Reliable)
	void CastAbilityByKeyServer(EPressedKey KeyType);

	/** Ÿ���� �� ��ų ��� */

	void CastAbilityAfterTargeting_WithAuthority();

	FORCEINLINE URPGAnimInstance* GetRPGAnimInstance() const { return RPGAnimInstance; }
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE int32 GetCurrentCombo() const { return CurrentCombo; }

protected:

	/** ��ų ��� �غ� */

	UFUNCTION(NetMulticast, Reliable)
	void CastAbilityByKeyMulticast(EPressedKey KeyType);

	virtual void CastAbilityByKey(EPressedKey KeyType);

	/** Ÿ���� �� ��ų ��� */

	UFUNCTION(Server, Reliable)
	void CastAbilityAfterTargetingServer();

	UFUNCTION(NetMulticast, Reliable)
	void CastAbilityAfterTargetingMulticast();

	virtual void CastAbilityAfterTargeting();

	void DrawTargetingCursor();

	void SpawnClickParticle(const FVector& EmitLocation);

	/** �̵� */

	void InitDestAndDir();

	UFUNCTION(Server, Reliable)
	void SetDestinaionAndPathServer(const FVector& HitLocation);

	void UpdateMovement();

	UFUNCTION()
	void OnRep_PathX();

	/** �Ϲ� ���� */

	void NormalAttackPressed();

	void NormalAttackWithCombo();

	UFUNCTION(Server, Reliable)
	void NormalAttackWithComboServer();

	UFUNCTION(NetMulticast, Reliable)
	void NormalAttackWithComboMulticast();

	void TurnTowardAttackPoint();

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackEndComboState();

	UFUNCTION()
	virtual void CastNormalAttack();

	/** ���� */

	void PlayerDie();

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLoc, const FRotator& SpawnRot = FRotator::ZeroRotator);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	ARPGPlayerController* RPGPlayerController;

	URPGAnimInstance* RPGAnimInstance;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AimCursor;

	bool bAiming = false;

private:

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Character | Click")
	UParticleSystem* ClickParticle;

	/** ���� */

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character | Status")
	float Health = 200.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxHealth = 200.f;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Character | Status")
	float Mana = 100.f;

	UPROPERTY(VisibleAnywhere, Category = "Character | Status")
	float MaxMana = 100.f;

	/** �̵� */

	UPROPERTY(ReplicatedUsing = OnRep_PathX)
	TArray<float> PathX;

	UPROPERTY(Replicated)
	TArray<float> PathY;

	bool bUpdateMovement = false;

	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;

	/** �Ϲ� ���� */

	bool bIsAttacking = false;

	bool bCanNextCombo = false;

	int32 CurrentCombo = 0;

protected:

	int32 MaxCombo = 4;

	UPROPERTY(Replicated)
	FHitResult TargetingHitResult;

	FHitResult GroundHitResult;

};

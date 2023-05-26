
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGBasePlayerCharacter.generated.h"

class ARPGPlayerController;
class URPGAnimInstance;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RPG_API ARPGBasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ARPGBasePlayerCharacter();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

public:	

	void StopMove();

	void SetDestinationAndPath();

	void DoNormalAttack();

protected:

	void SpawnClickParticle(const FVector& EmitLocation);

	/** 이동 */

	void InitDestAndDir();

	UFUNCTION(Server, Reliable)
	void SetDestinaionAndPathServer(const FVector& HitLocation);

	void UpdateMovement();

	UFUNCTION()
	void OnRep_PathX();

	/** 일반 공격 */

	void NormalAttackPressed();

	void NormalAttackWithCombo(const FVector& AttackPoint);

	UFUNCTION(Server, Reliable)
	void NormalAttackWithComboServer(const FVector& AttackPoint);

	UFUNCTION(NetMulticast, Reliable)
	void NormalAttackWithComboMulticast(const FVector& AttackPoint);

	void TurnTowardAttackPoint(const FVector& AttackPoint);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void AttackEndComboState();

	UFUNCTION()
	void NormalAttackNextCombo();

	/** 죽음 */

	void PlayerDie();

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	ARPGPlayerController* RPGPlayerController;

	URPGAnimInstance* RPGAnimInstance;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Click")
	UParticleSystem* ClickParticle;

	UPROPERTY(ReplicatedUsing = OnRep_PathX)
	TArray<float> PathX;

	UPROPERTY(Replicated)
	TArray<float> PathY;

	/** 이동 관련 */

	bool bUpdateMovement = false;

	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;

	/** 일반 공격 */

	bool bIsAttacking = false;

	bool bCanNextCombo = false;

	int32 CurrentCombo = 0;

	int32 MaxCombo = 4;
};


#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "../RPG.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

ARPGBasePlayerCharacter::ARPGBasePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetUsingAbsoluteRotation(true);
	CameraArm->TargetArmLength = 1200.f;
	CameraArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraArm->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	//GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	//GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerBody);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttack, ECollisionResponse::ECR_Block);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	AttackEndComboState();
}

void ARPGBasePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGBasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	RPGAnimInstance->DOnAttackInputCheck.AddUFunction(this, FName("NormalAttackNextCombo"));
	RPGAnimInstance->OnMontageEnded.AddDynamic(this, &ARPGBasePlayerCharacter::OnAttackMontageEnded);
}

void ARPGBasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARPGBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGBasePlayerCharacter::DoNormalAttack(const FVector& AttackPoint)
{
	PlayAttackEffectServer(AttackPoint);
}

void ARPGBasePlayerCharacter::NormalAttackWithCombo(const FVector& AttackPoint)
{
	TurnTowardAttackPoint(AttackPoint);

	if (bIsAttacking)
	{
		if (FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo) == false) return;
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		if (CurrentCombo == 0)
		{
			AttackStartComboState();
			RPGAnimInstance->PlayNormalAttackMontage();
			RPGAnimInstance->JumpToAttackMontageSection(CurrentCombo);
			bIsAttacking = true;
		}
	}
}

void ARPGBasePlayerCharacter::PlayAttackEffectServer_Implementation(const FVector& AttackPoint)
{
	PlayAttackEffectMulticast(AttackPoint);
}

void ARPGBasePlayerCharacter::PlayAttackEffectMulticast_Implementation(const FVector& AttackPoint)
{
	NormalAttackWithCombo(AttackPoint);
}

void ARPGBasePlayerCharacter::TurnTowardAttackPoint(const FVector& AttackPoint)
{
	const FVector LookAtPoint(AttackPoint.X, AttackPoint.Y, GetActorLocation().Z);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookAtPoint));
}

void ARPGBasePlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bIsAttacking == false || CurrentCombo == 0) return;
	bIsAttacking = false;
	AttackEndComboState();
}

void ARPGBasePlayerCharacter::PlayerDie()
{

}

void ARPGBasePlayerCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void ARPGBasePlayerCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}

void ARPGBasePlayerCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ARPGBasePlayerCharacter::NormalAttackNextCombo()
{
	CanNextCombo = false;

	if (IsComboInputOn)
	{
		AttackStartComboState();
		RPGAnimInstance->JumpToAttackMontageSection(CurrentCombo);
	}
}


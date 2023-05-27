
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerBody);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttack, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	AttackEndComboState();
}

void ARPGBasePlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakeAnyDamage.AddDynamic(this, &ARPGBasePlayerCharacter::TakeAnyDamage);
}

void ARPGBasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	RPGAnimInstance->DOnAttackInputCheck.AddUFunction(this, FName("NormalAttackNextCombo"));
	RPGAnimInstance->OnMontageEnded.AddDynamic(this, &ARPGBasePlayerCharacter::OnAttackMontageEnded);
}

void ARPGBasePlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (Health == 0.f) return;

	Health = FMath::Max(Health - Damage, 0.f);
	DOnChangeHealthPercentage.Broadcast(Health / MaxHealth);

	if (Health == 0.f)
	{
		PlayerDie();
	}
	else
	{
		// TODO : ÀÌÆåÆ® Àç»ý
	}
}

void ARPGBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateMovement)
	{
		UpdateMovement();
	}
}

void ARPGBasePlayerCharacter::StopMove()
{
	GetMovementComponent()->StopMovementImmediately();
	bUpdateMovement = false;
	PathIdx = 0;
}

void ARPGBasePlayerCharacter::SetDestinationAndPath()
{
	FHitResult Hit;
	Cast<APlayerController>(GetController())->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	
	if (Hit.bBlockingHit == false)
	{
		WLOG(TEXT("Nothing Hit"));
		return;
	}

	SpawnClickParticle(Hit.ImpactPoint);

	if (HasAuthority())
	{
		GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetActorLocation(), Hit.ImpactPoint, PathX, PathY);
		InitDestAndDir();
	}
	else
	{
		SetDestinaionAndPathServer(Hit.ImpactPoint);
	}

	/*if (HasAuthority())
	{
		for (int32 i = 0; i < PathX.Num(); i++)
		{
			DrawDebugPoint(GetWorld(), FVector(PathX[i], PathY[i], 10.f), 10.f, FColor::Blue, false, 2.f);
		}
	}*/
}

void ARPGBasePlayerCharacter::DoNormalAttack()
{
	FHitResult GroundHit, EnemyHit;
	Cast<APlayerController>(GetController())->GetHitResultUnderCursor(ECC_Visibility, false, GroundHit);
	//GetHitResultUnderCursor(ECC_GroundTrace, false, GroundHit);
	//GetHitResultUnderCursor(ECC_EnemyTrace, false, EnemyHit);

	if (GroundHit.bBlockingHit)
	{
		if (HasAuthority())
		{
			NormalAttackWithCombo(GroundHit.ImpactPoint);
		}
		else
		{
			NormalAttackWithComboServer(GroundHit.ImpactPoint);
		}

		SpawnClickParticle(GroundHit.ImpactPoint);
	}
}

void ARPGBasePlayerCharacter::Ability_Q()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayAbility_Q_Montage();
}

void ARPGBasePlayerCharacter::Ability_W()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayAbility_W_Montage();
}

void ARPGBasePlayerCharacter::Ability_E()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayAbility_E_Montage();
}

void ARPGBasePlayerCharacter::Ability_R()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayAbility_R_Montage();
}

void ARPGBasePlayerCharacter::SpawnClickParticle(const FVector& EmitLocation)
{
	if (ClickParticle == nullptr) return;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ClickParticle, EmitLocation);
}

void ARPGBasePlayerCharacter::InitDestAndDir()
{
	bUpdateMovement = true;
	NextPoint = FVector(PathX[0], PathY[0], GetActorLocation().Z);
	NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
}

void ARPGBasePlayerCharacter::SetDestinaionAndPathServer_Implementation(const FVector& HitLocation)
{
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetActorLocation(), HitLocation, PathX, PathY);
}

void ARPGBasePlayerCharacter::UpdateMovement()
{
	if (FVector::Dist(NextPoint, GetActorLocation()) > 20.f)
	{
		AddMovementInput(NextDirection);
	}
	else
	{
		PathIdx++;
		if (PathIdx == PathX.Num())
		{
			bUpdateMovement = false;
			PathIdx = 0;
		}
		else
		{
			NextPoint = FVector(PathX[PathIdx], PathY[PathIdx], GetActorLocation().Z);
			NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
		}
	}
}

void ARPGBasePlayerCharacter::OnRep_PathX()
{
	InitDestAndDir();
}

void ARPGBasePlayerCharacter::NormalAttackWithComboServer_Implementation(const FVector& AttackPoint)
{
	NormalAttackWithComboMulticast(AttackPoint);
}

void ARPGBasePlayerCharacter::NormalAttackWithComboMulticast_Implementation(const FVector& AttackPoint)
{
	NormalAttackWithCombo(AttackPoint);
}

void ARPGBasePlayerCharacter::NormalAttackWithCombo(const FVector& AttackPoint)
{
	TurnTowardAttackPoint(AttackPoint);

	if (bIsAttacking) return;
	if (bCanNextCombo)
	{
		CurrentCombo = (CurrentCombo + 1) % MaxCombo;
		bCanNextCombo = false;
	}
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayNormalAttackMontage();
	RPGAnimInstance->JumpToAttackMontageSection(CurrentCombo+1);
	bIsAttacking = true;
}

void ARPGBasePlayerCharacter::TurnTowardAttackPoint(const FVector& AttackPoint)
{
	const FVector LookAtPoint(AttackPoint.X, AttackPoint.Y, GetActorLocation().Z);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookAtPoint));
	GetCharacterMovement()->FlushServerMoves();
}

void ARPGBasePlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsAttacking = false;
	AttackEndComboState();
}

void ARPGBasePlayerCharacter::AttackEndComboState()
{
	bCanNextCombo = false;
	CurrentCombo = 0;
}

void ARPGBasePlayerCharacter::NormalAttackNextCombo()
{
	bCanNextCombo = true;
	bIsAttacking = false;
}

void ARPGBasePlayerCharacter::PlayerDie()
{

}

void ARPGBasePlayerCharacter::OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void ARPGBasePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBasePlayerCharacter, PathX);
	DOREPLIFETIME(ARPGBasePlayerCharacter, PathY);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Health);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Mana);
}

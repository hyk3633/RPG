
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
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerBody);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttack, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	//GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	AimCursor = CreateDefaultSubobject<UStaticMeshComponent>("Aim Cursor");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMeshAsset.Succeeded()) { AimCursor->SetStaticMesh(StaticMeshAsset.Object); }
	AimCursor->SetupAttachment(RootComponent);
	AimCursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AimCursor->SetCastShadow(false);
	AimCursor->SetVisibility(false);

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
	if (RPGAnimInstance)
	{
		RPGAnimInstance->DOnAttackInputCheck.AddUFunction(this, FName("CastNormalAttack"));
		RPGAnimInstance->OnMontageEnded.AddDynamic(this, &ARPGBasePlayerCharacter::OnAttackMontageEnded);
		RPGAnimInstance->SetMaxCombo(MaxCombo);
	}
}

void ARPGBasePlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	PLOG(TEXT("Player Take Damage : %f"), Damage);
	if (Health == 0.f) return;
	Health = FMath::Max(Health - Damage, 0.f);
	DOnChangeHealthPercentage.Broadcast(Health / MaxHealth);

	if (Health == 0.f)
	{
		PlayerDie();
	}
	else
	{
		// TODO : 이펙트 재생
	}
}

void ARPGBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateMovement)
	{
		UpdateMovement();
	}
	// 바닥에만 붙어 있도록
	if (bAiming && IsLocallyControlled())
	{
		DrawTargetingCursor();
	}

	if (bZooming)
	{
		CameraArm->TargetArmLength = (int32) (FMath::FInterpTo(CameraArm->TargetArmLength, NextArmLength, DeltaTime, 10) + 0.5f);
		if (FMath::Abs(CameraArm->TargetArmLength - NextArmLength) <= 6.f)
		{
			CameraArm->TargetArmLength = NextArmLength;
			bZooming = false;
		}
	}
}

void ARPGBasePlayerCharacter::DrawTargetingCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr) return;
	FHitResult TraceHitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	TraceHitResult.Location.Z += 5.f;
	AimCursor->SetWorldLocation(TraceHitResult.Location);
}

void ARPGBasePlayerCharacter::CameraZoomInOut(int8 Value)
{
	NextArmLength = FMath::Clamp(CameraArm->TargetArmLength + Value * 300, 600, 2100);
	bZooming = true;
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
	if (Hit.bBlockingHit == false) return;

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

/** 일반 공격 */

void ARPGBasePlayerCharacter::DoNormalAttack()
{
	// TODO : 바닥, 적 구분
	GetHitCursorClient();
	NormalAttackWithComboServer();
	SpawnClickParticle(TargetingHitResult.ImpactPoint);
}

void ARPGBasePlayerCharacter::GetHitCursorClient_Implementation()
{
	Cast<APlayerController>(GetController())->GetHitResultUnderCursor(ECC_Visibility, false, TargetingHitResult);
	GetHitCursorServer(TargetingHitResult);
}

void ARPGBasePlayerCharacter::GetHitCursorServer_Implementation(const FHitResult& Hit)
{
	// 서버에서만?
	GetHitCursorMulticast(Hit);
}

void ARPGBasePlayerCharacter::GetHitCursorMulticast_Implementation(const FHitResult& Hit)
{
	TargetingHitResult = Hit;
}

/** 스킬 사용 준비 */

void ARPGBasePlayerCharacter::CastAbilityByKeyServer_Implementation(EPressedKey KeyType)
{
	CastAbilityByKeyMulticast(KeyType);
}

void ARPGBasePlayerCharacter::CastAbilityByKeyMulticast_Implementation(EPressedKey KeyType)
{
	CastAbilityByKey(KeyType);
}

void ARPGBasePlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->SetCurrentState(KeyType);
}

/** 타게팅 후 스킬 사용 */

void ARPGBasePlayerCharacter::CastAbilityAfterTargeting_WithAuthority()
{
	if (RPGAnimInstance == nullptr) return;
	
	GetHitCursorClient();
	CastAbilityAfterTargetingServer();
	if (IsLocallyControlled()) AimCursor->SetVisibility(false);
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargetingServer_Implementation()
{
	CastAbilityAfterTargetingMulticast();
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargetingMulticast_Implementation()
{
	CastAbilityAfterTargeting();
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargeting()
{
	if (RPGAnimInstance == nullptr) return;
	bAiming = false;
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

void ARPGBasePlayerCharacter::NormalAttackWithComboServer_Implementation()
{
	NormalAttackWithComboMulticast();
}

void ARPGBasePlayerCharacter::NormalAttackWithComboMulticast_Implementation()
{
	NormalAttackWithCombo();
}

void ARPGBasePlayerCharacter::NormalAttackWithCombo()
{
	TurnTowardAttackPoint();

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

void ARPGBasePlayerCharacter::TurnTowardAttackPoint()
{
	const FVector LookAtPoint(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, GetActorLocation().Z);
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

void ARPGBasePlayerCharacter::CastNormalAttack()
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

void ARPGBasePlayerCharacter::SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	if (Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, SpawnLoc, SpawnRot);
	}
}

void ARPGBasePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBasePlayerCharacter, PathX);
	DOREPLIFETIME(ARPGBasePlayerCharacter, PathY);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Health);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Mana);
	DOREPLIFETIME(ARPGBasePlayerCharacter, TargetingHitResult);
}

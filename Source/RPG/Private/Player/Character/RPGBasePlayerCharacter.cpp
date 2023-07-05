
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "Player/RPGPlayerController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
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

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetUsingAbsoluteRotation(true);
	CameraArm->TargetArmLength = 1200.f;
	CameraArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraArm->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerBody);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttack, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	TargetingComp = CreateDefaultSubobject<USphereComponent>(TEXT("Targeting Component"));
	TargetingComp->SetupAttachment(RootComponent);
	TargetingComp->SetCollisionObjectType(ECC_PlayerAttack);
	TargetingComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TargetingComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TargetingComp->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Overlap);
	TargetingComp->SetSphereRadius(300.f);
	TargetingComp->SetRelativeLocation(FVector::ZeroVector);

	AimCursor = CreateDefaultSubobject<UStaticMeshComponent>("Aim Cursor");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMeshAsset.Succeeded()) { AimCursor->SetStaticMesh(StaticMeshAsset.Object); }
	AimCursor->SetupAttachment(RootComponent);
	AimCursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AimCursor->SetCastShadow(false);
	AimCursor->SetVisibility(false);

	RemainedCooldownTime.Init(0, 4);
	MaxCooldownTime.Init(0, 4);
	ManaUsage.Init(0, 4);

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

	if (IsLocallyControlled())
	{
		TargetingComp->OnComponentBeginOverlap.AddDynamic(this, &ARPGBasePlayerCharacter::OnTargetingComponentBeginOverlap);
		TargetingComp->OnComponentEndOverlap.AddDynamic(this, &ARPGBasePlayerCharacter::OnTargetingComponentEndOverlap);
	}
	RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	if (RPGAnimInstance)
	{
		RPGAnimInstance->DOnAttackInputCheck.AddUFunction(this, FName("CastNormalAttack"));
		RPGAnimInstance->DOnAttackEnded.AddUFunction(this, FName("OnAttackMontageEnded"));
		RPGAnimInstance->DOnDeathEnded.AddUFunction(this, FName("AfterDeath"));
		RPGAnimInstance->DOnAbilityMontageEnded.AddUFunction(this, FName("OnAbilityEnded"));
		RPGAnimInstance->SetMaxCombo(MaxCombo);
	}
}

void ARPGBasePlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	//PLOG(TEXT("Player Take Damage : %f"), Damage);
	if (HasAuthority() && Health)
	{
		Health = FMath::Max(Health - Damage, 0.f);
		if (Health == 0.f)
		{
			TempController = GetController();
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetWorldTimerManager().SetTimer(RespawnTimer, this, &ARPGBasePlayerCharacter::PlayerRespawn, 5.f);
			SetLifeSpan(6.f);
		}
	}
}

void ARPGBasePlayerCharacter::OnRep_Health()
{
	if (IsLocallyControlled())
	{
		DOnChangeHealthPercentage.Broadcast(Health / MaxHealth);
	}
	if (Health == 0.f)
	{
		PlayerDie();
	}
}

/** --------------------------- 죽음 --------------------------- */

void ARPGBasePlayerCharacter::PlayerRespawn()
{
	DetachFromControllerPendingDestroy();
	GetWorld()->GetAuthGameMode()->RestartPlayer(TempController);
}

void ARPGBasePlayerCharacter::PlayerDie()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayDeathMontage();
}

void ARPGBasePlayerCharacter::AfterDeath()
{
	GetMovementComponent()->Deactivate();
}

void ARPGBasePlayerCharacter::UsingMana(EPressedKey KeyType)
{
	const int8 Index = StaticCast<int8>(KeyType);
	Mana = FMath::Clamp(Mana - ManaUsage[Index], 0, MaxMana);
}

void ARPGBasePlayerCharacter::OnRep_Mana()
{
	if (IsLocallyControlled())
	{
		DOnChangeManaPercentage.Broadcast(Mana / MaxMana);
	}
}

void ARPGBasePlayerCharacter::OnTargetingComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OtherActor);
	if (Enemy)
	{
		Enemy->OnRenderCustomDepthEffect(126);
		OutlinedEnemies.Add(Enemy);
	}
}

void ARPGBasePlayerCharacter::OnTargetingComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OtherActor);
	if (Enemy)
	{
		Enemy->GetMesh()->SetRenderCustomDepth(false);
		OutlinedEnemies.Remove(Enemy);
	}
}

void ARPGBasePlayerCharacter::RecoveryHealth(const int32 RecoveryAmount)
{
	Health = FMath::Clamp(Health + RecoveryAmount, 0, MaxHealth);
}

void ARPGBasePlayerCharacter::RecoveryMana(const int32 RecoveryAmount)
{
	Mana = FMath::Clamp(Mana + RecoveryAmount, 0, MaxMana);
}

void ARPGBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateMovement)
	{
		UpdateMovement();
	}
	
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

	if (HasAuthority() && AbilityCooldownBit)
	{
		for (int8 idx = 0; idx < 4; idx++)
		{
			if (AbilityCooldownBit & (1 << idx))
			{
				RemainedCooldownTime[idx] = FMath::Clamp(RemainedCooldownTime[idx] - DeltaTime, 0, MaxCooldownTime[idx]);
				if (RemainedCooldownTime[idx] == 0)
				{
					AbilityCooldownBit &= ~(1 << idx);
					AbilityCooldownEndClient(idx);
				}
			}
		}
	}

	if (IsLocallyControlled())
	{
		//PLOG(TEXT("%d"),IsAbilityAvailable(EPressedKey::EPK_Q));
	}
}

void ARPGBasePlayerCharacter::DrawTargetingCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr) return;
	
	PC->GetHitResultUnderCursor(ECC_Visibility, true, CursorHitResult);
	FVector CursorLocation = CursorHitResult.Location;
	CursorLocation.Z = GetActorLocation().Z;
	CursorHitResult.Location.Z += 5.f;
	if (FVector::Dist2D(GetActorLocation(), CursorLocation) >= 800.f)
	{
		CursorLocation = GetActorLocation() + (CursorLocation - GetActorLocation()).GetUnsafeNormal() * 800.f;
	}

	AimCursor->SetWorldLocation(CursorLocation);
	TargetingComp->SetWorldLocation(CursorLocation);
}

void ARPGBasePlayerCharacter::CameraZoomInOut(int8 Value)
{
	NextArmLength = FMath::Clamp(CameraArm->TargetArmLength + Value * 300, 600, 2100);
	bZooming = true;
}

/** --------------------------- 이동 --------------------------- */

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

void ARPGBasePlayerCharacter::SpawnClickParticle(const FVector& EmitLocation)
{
	if (ClickParticle == nullptr) return;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ClickParticle, EmitLocation);
}

/** --------------------------- 일반 공격 --------------------------- */

void ARPGBasePlayerCharacter::DoNormalAttack()
{
	// TODO : 바닥, 적 구분
	if (IsLocallyControlled())
	{
		GetHitCursor();
		NormalAttackWithComboServer();
		SpawnClickParticle(TargetingHitResult.ImpactPoint);
	}
}

void ARPGBasePlayerCharacter::GetHitCursor()
{
	Cast<APlayerController>(GetController())->GetHitResultUnderCursor(ECC_Visibility, false, TargetingHitResult);
	GetHitCursorServer(TargetingHitResult);
}

void ARPGBasePlayerCharacter::GetHitCursorServer_Implementation(const FHitResult& Hit)
{
	GetHitCursorMulticast(Hit);
}

void ARPGBasePlayerCharacter::GetHitCursorMulticast_Implementation(const FHitResult& Hit)
{
	TargetingHitResult = Hit;
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
	if (RPGAnimInstance)
	{
		RPGAnimInstance->PlayNormalAttackMontage();
		RPGAnimInstance->JumpToAttackMontageSection(CurrentCombo + 1);
		bIsAttacking = true;
	}
}

void ARPGBasePlayerCharacter::TurnTowardAttackPoint()
{
	const FVector LookAtPoint(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, GetActorLocation().Z);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookAtPoint));
	GetCharacterMovement()->FlushServerMoves();
}

void ARPGBasePlayerCharacter::OnAttackMontageEnded()
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

/** --------------------------- 스킬 사용 준비 --------------------------- */

void ARPGBasePlayerCharacter::ReadyToCastAbilityByKey(EPressedKey KeyType)
{
	CastAbilityByKeyServer(KeyType);
}

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
	RPGAnimInstance->SetCurrentKeyState(KeyType);
}

void ARPGBasePlayerCharacter::TargetingCompOn()
{
	TargetingComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ARPGBasePlayerCharacter::TargetingCompOff()
{
	TargetingComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TargetingComp->SetRelativeLocation(FVector::ZeroVector);
	for (ARPGBaseEnemyCharacter* Enemy : OutlinedEnemies)
	{
		Enemy->OffRenderCustomDepthEffect();
	}
	OutlinedEnemies.Empty();
}

/** --------------------------- 스킬 사용 취소 --------------------------- */

void ARPGBasePlayerCharacter::CancelAbility()
{
	if (IsLocallyControlled())
	{
		bAiming = false;
		AimCursor->SetVisibility(false);
	}
}

/** --------------------------- 타게팅 후 스킬 사용 --------------------------- */

void ARPGBasePlayerCharacter::GetCursorHitResultCastAbility()
{
	if (RPGAnimInstance == nullptr) return;
	
	GetHitCursor();
	CastAbilityAfterTargetingServer();
	if (IsLocallyControlled()) AimCursor->SetVisibility(false);
	if (bUpdateMovement)
	{
		StopMove();
	}
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargetingServer_Implementation()
{
	CastAbilityAfterTargetingMulticast();
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargetingMulticast_Implementation()
{
	CastAbilityAfterTargeting();
	TurnTowardAttackPoint();
}

void ARPGBasePlayerCharacter::CastAbilityAfterTargeting()
{
	if (RPGAnimInstance == nullptr) return;
	if (TargetingHitResult.bBlockingHit == false) return;
	if (HasAuthority()) return;
	if (IsLocallyControlled())
	{
		bAiming = false;
	}
}

void ARPGBasePlayerCharacter::AbilityActiveBitSet(EPressedKey KeyType)
{
	AbilityActiveBit |= (1 << StaticCast<int8>(KeyType));
}

void ARPGBasePlayerCharacter::AbilityActiveBitOffClient_Implementation(EPressedKey KeyType)
{
	AbilityActiveBitOff(KeyType);
}

void ARPGBasePlayerCharacter::AbilityActiveBitOff(EPressedKey KeyType)
{
	AbilityActiveBit &= ~(1 << StaticCast<int8>(KeyType));
}

void ARPGBasePlayerCharacter::AbilityCooldownStartServer_Implementation(EPressedKey KeyType)
{
	AbilityCooldownStart(KeyType);
}

void ARPGBasePlayerCharacter::AbilityCooldownStart(EPressedKey KeyType)
{
	const int8 ShiftNumber = StaticCast<int8>(KeyType);
	AbilityCooldownBit |= (1 << ShiftNumber);
	RemainedCooldownTime[ShiftNumber] = MaxCooldownTime[ShiftNumber];
}

void ARPGBasePlayerCharacter::AbilityCooldownEndClient_Implementation(int8 Bit)
{
	DOnAbilityCooldownEnd.Broadcast(Bit);
}

void ARPGBasePlayerCharacter::SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	if (Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, SpawnLoc, SpawnRot);
	}
}

float ARPGBasePlayerCharacter::GetCooldownPercentage(int8 Bit) const
{
	return 1 - (RemainedCooldownTime[Bit] / MaxCooldownTime[Bit]);
}

bool ARPGBasePlayerCharacter::IsAbilityAvailable(EPressedKey KeyType)
{
	const bool bIsActive = !(AbilityActiveBit & (1 << StaticCast<uint8>(KeyType)));
	const bool bIsCooldownEnd = !(AbilityCooldownBit & (1 << StaticCast<uint8>(KeyType)));
	return (bIsActive && bIsCooldownEnd);
}

bool ARPGBasePlayerCharacter::GetIsAnyMontagePlaying() const
{
	if (RPGAnimInstance == nullptr) return false;
	return RPGAnimInstance->IsAnyMontagePlaying();
}

bool ARPGBasePlayerCharacter::GetAbilityERMontagePlaying()
{
	if (RPGAnimInstance == nullptr) return false;
	return RPGAnimInstance->GetIsAbilityERMontagePlaying();
}

void ARPGBasePlayerCharacter::SetAbilityCooldownTime(int8 QTime, int8 WTime, int8 ETime, int8 RTime)
{
	MaxCooldownTime[0] = QTime;
	MaxCooldownTime[1] = WTime;
	MaxCooldownTime[2] = ETime;
	MaxCooldownTime[3] = RTime;
}

void ARPGBasePlayerCharacter::SetAbilityManaUsage(int32 QUsage, int32 WUsage, int32 EUsage, int32 RUsage)
{
	ManaUsage[0] = QUsage;
	ManaUsage[1] = WUsage;
	ManaUsage[2] = EUsage;
	ManaUsage[3] = RUsage;
}

void ARPGBasePlayerCharacter::GetTargetingCompOverlappingEnemies(TArray<AActor*>& Enemies)
{
	TargetingComp->GetOverlappingActors(Enemies);
}

void ARPGBasePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBasePlayerCharacter, PathX);
	DOREPLIFETIME(ARPGBasePlayerCharacter, PathY);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Health);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Mana);
	DOREPLIFETIME(ARPGBasePlayerCharacter, TargetingHitResult);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, AbilityCooldownBit, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, RemainedCooldownTime, COND_OwnerOnly);
}

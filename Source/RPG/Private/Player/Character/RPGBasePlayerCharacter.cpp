
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "Player/RPGPlayerController.h"
#include "Player/RPGPlayerState.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "GameSystem/ObstacleChecker.h"
#include "DataAsset/MapNavDataAsset.h"
#include "DamageType/DamageTypeStunAndPush.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "AssetRegistry/AssetRegistryModule.h"

ARPGBasePlayerCharacter::ARPGBasePlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 640.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Arm Component"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetUsingAbsoluteRotation(true);
	CameraArm->TargetArmLength = 1200.f;
	CameraArm->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraArm->bDoCollisionTest = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MinimapArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Minimap Arm"));
	MinimapArm->SetupAttachment(RootComponent);
	MinimapArm->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	MinimapArm->TargetArmLength = 300.f;
	MinimapArm->bInheritPitch = false;
	MinimapArm->bInheritYaw = false;
	MinimapArm->bInheritRoll = false;

	MinimapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Minimap Capture"));
	MinimapCapture->SetupAttachment(MinimapArm);
	MinimapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCapture->OrthoWidth = 1000.f;
	MinimapCapture->ShowFlags.SkeletalMeshes = 0;
	MinimapCapture->ShowFlags.Particles = 0;
	MinimapCapture->ShowFlags.Atmosphere = 0;
	MinimapCapture->ShowFlags.Fog = 0;
	MinimapCapture->ShowFlags.DynamicShadows = 0;
	MinimapCapture->ShowFlags.Decals = 0;

	PlayerIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite"));
	PlayerIconSprite->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UPaperSprite> PlayerIconAsset(TEXT("PaperSprite'/Game/_Assets/Texture2D/Minimap/PlayerIcon_Sprite.PlayerIcon_Sprite'"));
	if (PlayerIconAsset.Succeeded()) { PlayerIconSprite->SetSprite(PlayerIconAsset.Object); }
	static ConstructorHelpers::FObjectFinder<UPaperSprite> OtherPlayerIconAsset(TEXT("PaperSprite'/Game/_Assets/Texture2D/Minimap/OtherPlayerIcon_Sprite.OtherPlayerIcon_Sprite'"));
	if (OtherPlayerIconAsset.Succeeded()) { OtherPlayerIcon = OtherPlayerIconAsset.Object; }
	PlayerIconSprite->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	PlayerIconSprite->SetRelativeLocation(FVector(0, 0, 299));
	PlayerIconSprite->SetRelativeScale3D(FVector(0.2));
	PlayerIconSprite->bOwnerNoSee = true;
	PlayerIconSprite->bVisibleInSceneCaptureOnly = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_ObstacleCheck, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Overlap);

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(60.f);
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PlayerBody);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Overlap);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyAttack, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_ObstacleCheck, ECollisionResponse::ECR_Ignore);

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
	SkillPowerCorrectionValues.Init(0, 4);
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

		MinimapRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 256, 256);
		if (MinimapRenderTarget)
		{
			MinimapCapture->TextureTarget = MinimapRenderTarget;
		}
		MinimapMatInstDynamic = GetMesh()->CreateDynamicMaterialInstance(0, MinimapMatInterface);
		MinimapMatInstDynamic->SetTextureParameterValue(FName("RenderTargetAsset"), MinimapRenderTarget);
	}
	RPGAnimInstance = Cast<URPGAnimInstance>(GetMesh()->GetAnimInstance());
	if (RPGAnimInstance)
	{
		RPGAnimInstance->DOnAttackInputCheck.AddUFunction(this, FName("CastNormalAttack"));
		RPGAnimInstance->DOnAnimMontageEnded.AddUFunction(this, FName("OnAttackMontageEnded"));
		RPGAnimInstance->DOnAnimMontageEnded.AddUFunction(this, FName("OnStunMontageEnded"));
		RPGAnimInstance->DOnAbilityMontageEnded.AddUFunction(this, FName("OnAbilityEnded"));
		RPGAnimInstance->SetMaxCombo(MaxCombo);
	}
	if (HasAuthority())
	{
		PlayerIconSprite->SetVisibility(false);
	}
	else if (IsLocallyControlled() == false && OtherPlayerIcon)
	{
		PlayerIconSprite->SetSprite(OtherPlayerIcon);
	}
}

void ARPGBasePlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (HasAuthority() && Health)
	{
		UDamageTypeBase* DT_Base = Cast<UDamageTypeBase>(const_cast<UDamageType*>(DamageType));
		if (DT_Base == nullptr) return;

		const int32 FinalDamage = DT_Base->CalculateDamage(Damage, CharacterDefensivePower + EquipmentDefensivePower);
		Health = FMath::Max(Health - FinalDamage, 0.f);
		PLOG(TEXT("Player Take Damage : %d"), FinalDamage);

		if (Health == 0.f)
		{
			DOnPlayerDeath.Broadcast(this);
			TempController = Cast<APlayerController>(GetController());
			SetCharacterDeadStateMulticast();
			GetWorldTimerManager().SetTimer(RespawnTimer, this, &ARPGBasePlayerCharacter::PlayerRespawn, 5.f);
			SetLifeSpan(5.2f);
		}
		else
		{
			UDamageTypeStunAndPush* DT_StunAndPush = Cast<UDamageTypeStunAndPush>(DT_Base);
			if (DT_StunAndPush)
			{
				DT_StunAndPush->GetPushed(DamageCauser, this);
				bStunned = true;
			}
		}
	}
}

void ARPGBasePlayerCharacter::OnRep_bStunned()
{
	if (bStunned && RPGAnimInstance)
	{
		RPGAnimInstance->PlayStunMontage();
		if (IsLocallyControlled())
		{
			CancelAbility();
			if(bUpdateMovement) StopMove();
		}
	}
}

void ARPGBasePlayerCharacter::OnStunMontageEnded(EMontageEnded MontageType)
{
	if (MontageType == EMontageEnded::EME_StunEnded)
	{
		bStunned = false;
		if (IsLocallyControlled()) StunEndServer();
	}
}

void ARPGBasePlayerCharacter::StunEndServer_Implementation()
{
	bStunned = false;
}

void ARPGBasePlayerCharacter::ApplyDamageToEnemy(APawn* TargetEnemy, const float& Damage, TSubclassOf<UDamageType> DamageType)
{
	UGameplayStatics::ApplyDamage(TargetEnemy, Damage, GetController(), this, DamageType);
}

void ARPGBasePlayerCharacter::OnTargetingComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OtherActor);
	if (IsValid(Enemy))
	{
		Enemy->OnRenderCustomDepthEffect(126);
		OutlinedEnemies.Add(Enemy);
	}
}

void ARPGBasePlayerCharacter::OnTargetingComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OtherActor);
	if (IsValid(Enemy))
	{
		Enemy->OffRenderCustomDepthEffect();
		OutlinedEnemies.Remove(Enemy);
	}
}

void ARPGBasePlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

/** --------------------------- 죽음 --------------------------- */

void ARPGBasePlayerCharacter::SetCharacterDeadStateMulticast_Implementation()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMovementComponent()->Deactivate();
}

void ARPGBasePlayerCharacter::PlayerRespawn()
{
	DetachFromControllerPendingDestroy();
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->SpawnPlayerCharacterAndPossess(TempController, CharacterType);
}

void ARPGBasePlayerCharacter::PlayerDie()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->PlayDeathMontage();
}

/** --------------------------- 체력, 마나 --------------------------- */

void ARPGBasePlayerCharacter::OnRep_Health()
{
	if (IsLocallyControlled())
	{
		DOnChangeHealthPercentage.Broadcast(Health / (CharacterMaxHP + EquipmentMaxHP));
	}
	if (Health == 0.f)
	{
		PlayerDie();
	}
}

void ARPGBasePlayerCharacter::UsingMana(EPressedKey KeyType)
{
	const int8 Index = StaticCast<int8>(KeyType);
	Mana = FMath::Clamp(Mana - ManaUsage[Index], 0, (CharacterMaxMP + EquipmentMaxMP));
}

void ARPGBasePlayerCharacter::OnRep_Mana()
{
	if (IsLocallyControlled())
	{
		DOnChangeManaPercentage.Broadcast(Mana / (CharacterMaxMP + EquipmentMaxMP));
	}
}

void ARPGBasePlayerCharacter::ResetHealthMana()
{
	Health = CharacterMaxHP + EquipmentMaxHP;
	Mana = CharacterMaxMP + EquipmentMaxMP;
}

void ARPGBasePlayerCharacter::ResetHealthManaUI()
{
	if (IsLocallyControlled())
	{
		DOnChangeHealthPercentage.Broadcast(Health / (CharacterMaxHP + EquipmentMaxHP));
		DOnChangeManaPercentage.Broadcast(Mana / (CharacterMaxMP + EquipmentMaxMP));
	}
}

void ARPGBasePlayerCharacter::RecoveryHealth(const int32 RecoveryAmount)
{
	Health = FMath::Clamp(Health + RecoveryAmount, 0, (CharacterMaxHP + EquipmentMaxHP));
}

void ARPGBasePlayerCharacter::RecoveryMana(const int32 RecoveryAmount)
{
	Mana = FMath::Clamp(Mana + RecoveryAmount, 0, (CharacterMaxMP + EquipmentMaxMP));
}

void ARPGBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateMovement)
	{
		if (IsLocallyControlled())
		{
			UpdateMovement();
		}
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
}

void ARPGBasePlayerCharacter::DrawTargetingCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr) return;
	
	PC->GetHitResultUnderCursor(ECC_GroundTrace, true, CursorHitResult);
	CursorHitResult.Location.Z = GetActorLocation().Z + 5;
	if (FVector::Dist2D(GetActorLocation(), CursorHitResult.Location) >= 800.f)
	{
		CursorHitResult.Location = GetActorLocation() + (CursorHitResult.Location - GetActorLocation()).GetUnsafeNormal() * 800.f;
	}

	AimCursor->SetWorldLocation(CursorHitResult.Location);
	TargetingComp->SetWorldLocation(CursorHitResult.Location);
}

void ARPGBasePlayerCharacter::CameraZoomInOut(int8 Value)
{
	NextArmLength = FMath::Clamp(CameraArm->TargetArmLength + Value * 300, 600, 2100);
	bZooming = true;
}

/** --------------------------- 이동 --------------------------- */

void ARPGBasePlayerCharacter::UpdateMovement()
{
	const float Dist = FVector::Dist(NextPoint, GetActorLocation());
	if (Dist > 20.f)
	{
		if (Dist > 55.f)
		{
			StopMove();
			SetDestinaionAndPathServer(Destination);
		}
		else
		{
			AddMovementInput(NextDirection * 200 * (CharacterDexterity + EquipmentDexterity));
		}
	}
	else
	{
		PathIdx++;
		if (PathIdx == PathArr.Num())
		{
			bUpdateMovement = false;
			PathIdx = 0;
		}
		else
		{
			NextPoint = FVector(PathArr[PathIdx].X, PathArr[PathIdx].Y, GetActorLocation().Z);
			NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
		}
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
	if (Hit.bBlockingHit == false) return;

	Destination = Hit.ImpactPoint;
	SpawnClickParticle(Destination);
	SetDestinaionAndPathServer(Destination);
}

void ARPGBasePlayerCharacter::SpawnClickParticle(const FVector& EmitLocation)
{
	if (ClickParticle == nullptr) return;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ClickParticle, EmitLocation);
}

void ARPGBasePlayerCharacter::SetDestinaionAndPathServer_Implementation(const FVector_NetQuantize& HitLocation)
{
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetActorLocation(), HitLocation, PathArr);
}

void ARPGBasePlayerCharacter::OnRep_PathArr()
{
	if (IsLocallyControlled())
	{
		if (PathArr.Num() == 0) return;
		InitDestAndDir();
	}
}

void ARPGBasePlayerCharacter::InitDestAndDir()
{
	bUpdateMovement = true;
	NextPoint = FVector(PathArr[0].X, PathArr[0].Y, GetActorLocation().Z);
	NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
}

void ARPGBasePlayerCharacter::InitDestAndDirServer_Implementation()
{
	InitDestAndDir();
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
	if (IsLocallyControlled()) return;
	TargetingHitResult = Hit;
}

void ARPGBasePlayerCharacter::NormalAttackWithComboServer_Implementation()
{
	NormalAttackWithCombo();
	PlayNormalAttackMontageMulticast(CurrentCombo + 1);
}

void ARPGBasePlayerCharacter::NormalAttackWithCombo()
{
	if (bIsAttacking) return;
	if (bCanNextCombo)
	{
		CurrentCombo = (CurrentCombo + 1) % MaxCombo;
		bCanNextCombo = false;
	}
	bIsAttacking = true;
}

void ARPGBasePlayerCharacter::PlayNormalAttackMontageMulticast_Implementation(const int32 Combo)
{
	TurnTowardAttackPoint();
	if (RPGAnimInstance && !HasAuthority())
	{
		RPGAnimInstance->PlayNormalAttackMontage();
		RPGAnimInstance->JumpToAttackMontageSection(Combo);
	}
}

void ARPGBasePlayerCharacter::TurnTowardAttackPoint()
{
	const FVector LookAtPoint(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, GetActorLocation().Z);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LookAtPoint));
	GetCharacterMovement()->FlushServerMoves();
}

void ARPGBasePlayerCharacter::OnAttackMontageEnded(EMontageEnded MontageType)
{
	if (MontageType == EMontageEnded::EME_AttackEnded && IsLocallyControlled())
	{
		AttackEndComboStateServer();
	}
}

void ARPGBasePlayerCharacter::AttackEndComboStateServer_Implementation()
{
	bIsAttacking = false;
	bCanNextCombo = false;
	CurrentCombo = 0;
}

void ARPGBasePlayerCharacter::CastNormalAttack()
{
	if (IsLocallyControlled())
	{
		CastNormalAttackServer();
	}
}

void ARPGBasePlayerCharacter::CastNormalAttackServer_Implementation()
{
	bCanNextCombo = true;
	bIsAttacking = false;
}

/** --------------------------- 스킬 사용 준비 --------------------------- */

void ARPGBasePlayerCharacter::ReadyToCastAbilityByKey(EPressedKey KeyType)
{
	ARPGPlayerController* PController = Cast<ARPGPlayerController>(GetController());
	if (PController) PController->bShowMouseCursor = false;
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

void ARPGBasePlayerCharacter::TargetingCompOn(const float& SphereRadius)
{
	const float Size = FMath::Clamp(SphereRadius / 30 - 2, 1, 20);
	AimCursor->SetWorldScale3D(FVector(Size, Size, 1));
	TargetingComp->SetSphereRadius(SphereRadius);
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
		ARPGPlayerController* PController = Cast<ARPGPlayerController>(GetController());
		if (PController) PController->bShowMouseCursor = true;
		bAiming = false;
		AimCursor->SetVisibility(false);
	}
}

/** --------------------------- 타게팅 후 스킬 사용 --------------------------- */

void ARPGBasePlayerCharacter::GetCursorHitResultCastAbility()
{
	if (RPGAnimInstance == nullptr) return;
	
	GetHitCursor();
	AimCursor->SetVisibility(false);
	if (bUpdateMovement) StopMove();
	ARPGPlayerController* PController = Cast<ARPGPlayerController>(GetController());
	if (PController) PController->bShowMouseCursor = true;
	bAiming = false;

	CastAbilityAfterTargetingServer();
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
	if (HasAuthority()) UsingMana(RPGAnimInstance->GetCurrentKeyState());
}

/** --------------------------- 스킬 쿨타임 계산 --------------------------- */

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

float ARPGBasePlayerCharacter::GetSkillPower(EPressedKey KeyType)
{
	return (CharacterSkillPower + EquipmentSkillPower) + SkillPowerCorrectionValues[StaticCast<int8>(KeyType)];
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

bool ARPGBasePlayerCharacter::IsAnyMontagePlaying() const
{
	if (RPGAnimInstance == nullptr) return false;
	return RPGAnimInstance->IsAnyMontagePlaying();
}

bool ARPGBasePlayerCharacter::IsNormalAttackMontagePlaying() const
{
	if (RPGAnimInstance == nullptr) return false;
	return RPGAnimInstance->IsNormalAttackMontagePlaying();
}

bool ARPGBasePlayerCharacter::IsAbilityERMontagePlaying()
{
	if (RPGAnimInstance == nullptr) return false;
	return RPGAnimInstance->IsAbilityERMontagePlaying();
}

/** --------------------------- 캐릭터 스탯 설정 --------------------------- */

void ARPGBasePlayerCharacter::InitCharacterStats(const FStatInfo& NewStats)
{
	CharacterDefensivePower = NewStats.DefenseivePower;
	CharacterDexterity		= NewStats.Dexterity;
	CharacterMaxHP			= NewStats.MaxHP;
	CharacterMaxMP			= NewStats.MaxMP;
	CharacterStrikingPower	= NewStats.StrikingPower;
	CharacterSkillPower		= NewStats.SkillPower;
	CharacterAttackSpeed	= NewStats.AttackSpeed;
}

void ARPGBasePlayerCharacter::SetEquipmentArmourStats(const float& Def, const float& Dex, const int32& MxHP, const int32& MxMP)
{
	SetEquipmentArmourStatsServer(Def, Dex, MxHP, MxMP);
}

bool ARPGBasePlayerCharacter::SetEquipmentArmourStatsServer_Validate(const float& Def, const float& Dex, const int32& MxHP, const int32& MxMP)
{
	if (Def < MIN_DEFENSIVE || Def > MAX_DEFENSIVE) return false;
	if (Dex < MIN_DEXTERITY || Dex > MAX_DEXTERITY) return false;
	if (MxHP < MIN_MAXHP || MxHP > MAX_MAXHP) return false;
	if (MxMP < MIN_MAXMP || MxMP > MAX_MAXMP) return false;
	return true;
}

void ARPGBasePlayerCharacter::SetEquipmentArmourStatsServer_Implementation(const float& Def, const float& Dex, const int32& MxHP, const int32& MxMP)
{
	EquipmentDefensivePower = Def;
	EquipmentDexterity		= Dex;
	EquipmentMaxHP			= MxHP;
	EquipmentMaxMP			= MxMP;
}

void ARPGBasePlayerCharacter::OnRep_CharacterDexterity()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->SetJogSpeed(CharacterDexterity + EquipmentDexterity);
}

void ARPGBasePlayerCharacter::OnRep_EquipmentDexterity()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->SetJogSpeed(CharacterDexterity + EquipmentDexterity);
}

void ARPGBasePlayerCharacter::OnRep_CharacterMaxHP()
{
	DOnChangeHealthPercentage.Broadcast(Health / (CharacterMaxHP + EquipmentMaxHP));
}

void ARPGBasePlayerCharacter::OnRep_EquipmentMaxHP()
{
	DOnChangeHealthPercentage.Broadcast(Health / (CharacterMaxHP + EquipmentMaxHP));
}

void ARPGBasePlayerCharacter::OnRep_CharacterMaxMP()
{
	DOnChangeManaPercentage.Broadcast(Mana / (CharacterMaxMP + EquipmentMaxMP));
}

void ARPGBasePlayerCharacter::OnRep_EquipmentMaxMP()
{
	DOnChangeManaPercentage.Broadcast(Mana / (CharacterMaxMP + EquipmentMaxMP));
}

void ARPGBasePlayerCharacter::SetEquipmentAccessoriesStats(const float& Stk, const float& Skp, const float& Atks)
{
	SetEquipmentAccessoriesStatsServer(Stk, Skp, Atks);
}

bool ARPGBasePlayerCharacter::SetEquipmentAccessoriesStatsServer_Validate(const float& Stk, const float& Skp, const float& Atks)
{
	if (Stk < MIN_STRIKINGPOWER || Stk > MAX_STRIKINGPOWER) return false;
	if (Skp < MIN_SKILLPOWER || Skp > MAX_SKILLPOWER) return false;
	if (Atks < MIN_ATTACKSPEED || Atks > MAX_ATTACKSPEED) return false;
	return true;
}

void ARPGBasePlayerCharacter::SetEquipmentAccessoriesStatsServer_Implementation(const float& Stk, const float& Skp, const float& Atks)
{
	EquipmentStrikingPower	= Stk;
	EquipmentSkillPower		= Skp;
	EquipmentAttackSpeed	= Atks;
}

void ARPGBasePlayerCharacter::OnRep_CharacterAttackSpeed()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->SetNormalAttackSpeed(CharacterAttackSpeed + EquipmentAttackSpeed);
}

void ARPGBasePlayerCharacter::OnRep_EquipmentAttackSpeed()
{
	if (RPGAnimInstance == nullptr) return;
	RPGAnimInstance->SetNormalAttackSpeed(CharacterAttackSpeed + EquipmentAttackSpeed);
}

void ARPGBasePlayerCharacter::InitializeEquipmentArmourStats()
{
	InitializeEquipmentArmourStatsServer();
}

void ARPGBasePlayerCharacter::InitializeEquipmentArmourStatsServer_Implementation()
{
	EquipmentDefensivePower	= 0;
	EquipmentDexterity		= 0;
	EquipmentMaxHP			= 0;
	EquipmentMaxMP			= 0;
}

void ARPGBasePlayerCharacter::InitializeEquipmentAccessoriesStats()
{
	InitializeEquipmentAccessoriesStatsServer();
}

void ARPGBasePlayerCharacter::InitializeEquipmentAccessoriesStatsServer_Implementation()
{
	EquipmentStrikingPower	= 0;
	EquipmentSkillPower		= 0;
	EquipmentAttackSpeed	= 0;
}

void ARPGBasePlayerCharacter::SetAbilityCooldownTime(const int8& QTime, const int8& WTime, const int8& ETime, const int8& RTime)
{
	MaxCooldownTime[0] = QTime;
	MaxCooldownTime[1] = WTime;
	MaxCooldownTime[2] = ETime;
	MaxCooldownTime[3] = RTime;
}

void ARPGBasePlayerCharacter::SetAbilityManaUsage(const int32& QUsage, const int32& WUsage, const int32& EUsage, const int32& RUsage)
{
	ManaUsage[0] = QUsage;
	ManaUsage[1] = WUsage;
	ManaUsage[2] = EUsage;
	ManaUsage[3] = RUsage;
}

void ARPGBasePlayerCharacter::SetSkillPowerCorrectionValues(const float& QPower, const float& WPower, const float& EPower, const float& RPower)
{
	SkillPowerCorrectionValues[0] = QPower;
	SkillPowerCorrectionValues[1] = WPower;
	SkillPowerCorrectionValues[2] = EPower;
	SkillPowerCorrectionValues[3] = RPower;
}

void ARPGBasePlayerCharacter::GetTargetingCompOverlappingEnemies(TArray<AActor*>& Enemies)
{
	TargetingComp->GetOverlappingActors(Enemies);
}

void ARPGBasePlayerCharacter::DrawDebugGrid()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath("MapNavDataAsset'/Game/_Assets/DataAsset/LowHeightTestAsset.LowHeightTestAsset'");
	if (AssetData.IsValid())
	{
		UMapNavDataAsset* MapNavDataAsset = Cast<UMapNavDataAsset>(AssetData.GetAsset());
		if (MapNavDataAsset == nullptr) return;

		int32 Count = 0;
		while (Count < MapNavDataAsset->GridWidthSize * MapNavDataAsset->GridLengthSize)
		{
			int32 X = MapNavDataAsset->FieldLocations[Count].X;
			int32 Y = MapNavDataAsset->FieldLocations[Count].Y;
			if (MapNavDataAsset->ExtraCost[Count] == 0)
				DrawDebugPoint(GetWorld(), FVector(X, Y, MapNavDataAsset->FieldHeights[Count] + 10.f), 5.f, FColor::Green, true);
			else if (MapNavDataAsset->ExtraCost[Count] < 8)
				DrawDebugPoint(GetWorld(), FVector(X, Y, MapNavDataAsset->FieldHeights[Count] + 10.f), 5.f, FColor::Yellow, true);
			else if (MapNavDataAsset->ExtraCost[Count] < 12)
				DrawDebugPoint(GetWorld(), FVector(X, Y, MapNavDataAsset->FieldHeights[Count] + 10.f), 5.f, FColor::Orange, true);
			else
				DrawDebugPoint(GetWorld(), FVector(X, Y, MapNavDataAsset->FieldHeights[Count] + 10.f), 5.f, FColor::Red, true);
			Count++;
		}
	}
}

void ARPGBasePlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, PathArr, COND_OwnerOnly);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Health);
	DOREPLIFETIME(ARPGBasePlayerCharacter, Mana);
	DOREPLIFETIME(ARPGBasePlayerCharacter, CharacterDexterity);
	DOREPLIFETIME(ARPGBasePlayerCharacter, EquipmentDexterity);
	DOREPLIFETIME(ARPGBasePlayerCharacter, CharacterAttackSpeed);
	DOREPLIFETIME(ARPGBasePlayerCharacter, EquipmentAttackSpeed);
	DOREPLIFETIME(ARPGBasePlayerCharacter, bStunned);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, CharacterMaxHP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, EquipmentMaxHP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, CharacterMaxMP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, EquipmentMaxMP, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, AbilityCooldownBit, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGBasePlayerCharacter, RemainedCooldownTime, COND_OwnerOnly);
}

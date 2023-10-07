
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "UI/RPGEnemyHealthBarWidget.h"
#include "Player/RPGPlayerController.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/EnemySpawner.h"
#include "DamageType/DamageTypeBase.h"
#include "DamageType/DamageTypeStunAndPush.h"
#include "DamageType/DamageTypeRestriction.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

ARPGBaseEnemyCharacter::ARPGBaseEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	AutoPossessAI = EAutoPossessAI::Spawned;

	GetMesh()->SetCollisionProfileName(FName("DeactivatedEnemyMesh"));
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->CustomDepthStencilValue = 251;
	GetMesh()->SetVisibility(false);
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionProfileName(FName("DeactivatedEnemyCapsule"));
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
	WeaponMesh->SetVisibility(false);

	EnemyIconSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Paper Sprite"));
	EnemyIconSprite->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UPaperSprite> EnemyIconAsset(TEXT("PaperSprite'/Game/_Assets/Texture2D/Minimap/EnemyIcon_Sprite.EnemyIcon_Sprite'"));
	if (EnemyIconAsset.Succeeded()) { EnemyIconSprite->SetSprite(EnemyIconAsset.Object); }
	EnemyIconSprite->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
	EnemyIconSprite->SetRelativeLocation(FVector(0, 0, 299));
	EnemyIconSprite->SetRelativeScale3D(FVector(0.1f));
	EnemyIconSprite->bVisibleInSceneCaptureOnly = true;
	EnemyIconSprite->SetVisibility(false);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Widget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/_Assets/Blueprints/HUD/WBP_EnemyHealthBar.WBP_EnemyHealthBar_C'"));
	if (WidgetAsset.Succeeded()) { HealthBarWidget->SetWidgetClass(WidgetAsset.Class); }
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBarWidget->SetDrawSize(FVector2D(200.f, 25.f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetVisibility(false);
}

void ARPGBaseEnemyCharacter::SetEnemyAssets(const FEnemyAssets& NewEnemyAssets)
{
	EnemyAssets = NewEnemyAssets;
	SetMeshAndController();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARPGBaseEnemyCharacter::OnCapsuleCollisionEvent);
}

void ARPGBaseEnemyCharacter::SetMeshAndController()
{
	GetMesh()->SetSkeletalMesh(EnemyAssets.BodyMesh);
	GetMesh()->SetAnimInstanceClass(EnemyAssets.AnimInstance);
	InitAnimInstance();
	AIControllerClass = EnemyAssets.AIController;

	if (EnemyAssets.WeaponMesh_Skeletal)
	{
		WeaponMesh->SetSkeletalMeshAsset(EnemyAssets.WeaponMesh_Skeletal);
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EnemyAssets.SocketName);
	}
}

void ARPGBaseEnemyCharacter::OnRep_EnemyAssets()
{
	SetMeshAndController();
	if (EnemyAssets.WeaponMesh_Static)
	{
		AttachWeaponStaticMesh(EnemyAssets.WeaponMesh_Static, EnemyAssets.SocketName);
	}
}

void ARPGBaseEnemyCharacter::InitAnimInstance()
{
	MyAnimInst = Cast<URPGEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	if (HasAuthority())
	{
		MyAnimInst->BindFunction();
		MyAnimInst->DOnAttack.AddUFunction(this, FName("Attack"));
		MyAnimInst->DOnAttackEnded.AddUFunction(this, FName("OnAttackMontageEnded"));
		MyAnimInst->DOnHitReactionEnded.AddUFunction(this, FName("OnHitReactionEnded"));
	}
}

void ARPGBaseEnemyCharacter::ActivateEnemy(const FVector& Location)
{
	Health = MaxHealth;
	bIsActivated = true;
	SetCollisionActivate();
	if (HasAuthority())
	{
		SetCollisionActivate();
		DOnActivate.Broadcast();
		OriginLocation = Location;
		SetActorLocation(Location);
		if (MySpawner)
		{
			DHandle = MySpawner->DOnPlayerOut.AddUFunction(this, FName("PlayerOut"));
		}
	}
}

void ARPGBaseEnemyCharacter::OnRep_bIsActivated()
{
	if (bIsActivated)
	{
		GetMesh()->SetVisibility(true);
		WeaponMesh->SetVisibility(true);
		MyAnimInst->CancelDeathMontage();
		EnemyIconSprite->SetVisibility(true);
	}
}

void ARPGBaseEnemyCharacter::SetCollisionActivate()
{
	GetMesh()->SetCollisionProfileName(FName("ActivatedEnemyMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(FName("ActivatedEnemyCapsule"));
}

void ARPGBaseEnemyCharacter::AttachWeaponStaticMesh(UStaticMesh* NewMesh, FName SocketName)
{
	UStaticMeshComponent* WeaponStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Static Mesh"));
	WeaponStaticMeshComponent->SetStaticMesh(NewMesh);
	WeaponStaticMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
}

void ARPGBaseEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakeAnyDamage.AddDynamic(this, &ARPGBaseEnemyCharacter::TakeAnyDamage);
}

void ARPGBaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	ProgressBar = Cast<URPGEnemyHealthBarWidget>(HealthBarWidget->GetWidget());
	if (ProgressBar) ProgressBar->EnemyHealthProgressBar->SetPercent(1.f);

	if (HasAuthority()) RPGGameMode = GetWorld()->GetAuthGameMode<ARPGGameModeBase>();
}

void ARPGBaseEnemyCharacter::PlayerOut(ACharacter* TargetPlayer)
{
	if (bIsActivated && TargetPlayer == GetTarget())
	{
		MyController->EmptyTheTarget();
		StopMovement();
	}
}

void ARPGBaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyAnimInst)
	{
		MyAnimInst->Speed = GetVelocity().Length();
		MyAnimInst->bIsInAir = GetMovementComponent()->IsFalling();
	}

	if (HasAuthority() && bUpdateMovement && bIsActivated)
	{
		//UpdateMovementFlowField();
		UpdateMovementAStar();
	}

	/*if (HasAuthority() && bIsActivated)
	{
		ctime += DeltaTime;
		if (ctime >= 0.1f)
		{
			RPGGameMode->DrawScore(GetActorLocation());
			ctime = 0.f;
		}
	}*/
}

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (HasAuthority() == false) return;

	UDamageTypeBase* DT_Base = Cast<UDamageTypeBase>(const_cast<UDamageType*>(DamageType));
	if (DT_Base == nullptr) return;
	
	const int32 FinalDamage = DT_Base->CalculateDamage(Damage, DefensivePower);
	HealthDecrease(FinalDamage);
	PLOG(TEXT("%s Enemy damaged : %d"), *DamagedActor->GetName(), FinalDamage);

	ARPGPlayerController* AttackerController = Cast<ARPGPlayerController>(InstigatorController);
	if (AttackerController)
	{
		AttackerController->ReceiveDamageInfo(GetMesh()->GetSocketTransform(FName("DamageSocket")).GetLocation(), FinalDamage);
	}

	SetMatInstDynamicMulticast(GetWorld()->TimeSeconds);

	if (Health != 0 && EnemyForm->GetEnemyType() != EEnemyType::EET_Boss)
	{
		UDamageTypeStunAndPush* DT_StunAndPush = Cast<UDamageTypeStunAndPush>(DT_Base);
		if (DT_StunAndPush)
		{
			DT_StunAndPush->GetPushed(DamageCauser, this);
			FalldownMulticast();
		}
		else
		{
			UDamageTypeRestriction* DT_Restriction = Cast<UDamageTypeRestriction>(DT_Base);
			if (DT_Restriction) StopActionMulticast();
			else
			{
				if (0.5f >= FMath::RandRange(0.f, 1.f))
				{
					if (bIsAttacking)
					{
						bIsAttacking = false;
						DOnAttackEnd.Broadcast();
					}
					HitReactionMulticast();
				}
			}
		}
	}
}

void ARPGBaseEnemyCharacter::SetMatInstDynamicMulticast_Implementation(float LastDamagedTime)
{
	if (HasAuthority()) return;
	
	if (MatInsts.Num() == 0)
	{
		int8 Idx = 0;
		MatInsts.Init(nullptr, GetMesh()->GetMaterials().Num());
		for (UMaterialInterface* Mat : GetMesh()->GetMaterials())
		{
			MatInsts[Idx++] = GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(Idx, Mat->GetMaterial());
		}
		MatInstsNum = Idx;
	}

	for (int8 Idx = 0; Idx < MatInstsNum; Idx++)
	{
		MatInsts[Idx]->SetScalarParameterValue(FName("LastDamagedTime"), GetWorld()->TimeSeconds);
	}
}

/** 체력 */

void ARPGBaseEnemyCharacter::HealthDecrease(const int32& Damage)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
	if (Health == 0)
	{
		if (PathCost.Num())
		{
			RPGGameMode->ClearEnemiesPathCost(PathCost);
			PathCost.Empty();
		}
		GetWorldTimerManager().ClearTimer(CheckTargetLocationTimer);
		GetWorldTimerManager().ClearTimer(HealthBarTimer);
		GetWorldTimerManager().ClearTimer(RestrictionTimer);
		GetWorldTimerManager().ClearTimer(FalldownTimer);
		DOnDeath.Broadcast();
		DOnDeactivate.Broadcast(EnemyType, this);
		DMoveEnd.Broadcast();
		MySpawner->DOnPlayerOut.Remove(DHandle);
		DisableSuckedInMulticast();
		MyAnimInst->PlayDeathMontage();
		if (bIsAttacking)
		{
			MyAnimInst->CancelAttackMontage();
			DOnAttackEnd.Broadcast();
			bIsAttacking = false;
		}
		SetCollisionDeactivate();
		bIsActivated = false;
		bUpdateMovement = false;
		RPGGameMode->SpawnItems(GetActorLocation());
		RespawnDelay();
	}
}

void ARPGBaseEnemyCharacter::RespawnDelay()
{
	GetWorldTimerManager().SetTimer(RespawnTimer, this, &ARPGBaseEnemyCharacter::EnemyRespawn, 5);
}

void ARPGBaseEnemyCharacter::EnemyRespawn()
{
	if (MySpawner)
	{
		MySpawner->EnemyRespawn();
	}
}

void ARPGBaseEnemyCharacter::OnRep_Health()
{
	OnHealthChanged();
}

void ARPGBaseEnemyCharacter::OnHealthChanged()
{
	if (ProgressBar == nullptr) return;
	
	ProgressBar->EnemyHealthProgressBar->SetPercent(Health / MaxHealth);

	if (Health > 0 && Health < MaxHealth)
	{
		HealthBarWidget->SetVisibility(true);
		GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ARPGBaseEnemyCharacter::HealthBarVisibilityOff, 60.f);
	}
	else if(Health == 0)
	{
		GetWorldTimerManager().SetTimer(HideMeshTimer, this, &ARPGBaseEnemyCharacter::HideMesh, 5.f);
		SetCollisionDeactivate();
		MyAnimInst->CancelAttackMontage();
		EnemyDeath();
	}
}

void ARPGBaseEnemyCharacter::HideMesh()
{
	GetMesh()->SetVisibility(false);
	WeaponMesh->SetVisibility(false);
}

void ARPGBaseEnemyCharacter::SetCollisionDeactivate()
{
	GetMesh()->SetCollisionProfileName(FName("DeactivatedEnemyMesh"));
	GetCapsuleComponent()->SetCollisionProfileName(FName("DeactivatedEnemyCapsule"));
}

void ARPGBaseEnemyCharacter::HealthBarVisibilityOff()
{
	HealthBarWidget->SetVisibility(false);
}

/** 죽음 */

void ARPGBaseEnemyCharacter::EnemyDeath()
{
	OffRenderCustomDepthEffect();
	HealthBarWidget->SetVisibility(false);
	EnemyIconSprite->SetVisibility(false);
	MyAnimInst->PlayDeathMontage();
}

/** 이동 */

void ARPGBaseEnemyCharacter::BTTask_Move()
{
	if (HasAuthority() && GetTarget())
	{
		bUpdateMovement = true;

		// AStar 방식
		GetWorldTimerManager().SetTimer(CheckTargetLocationTimer, this, &ARPGBaseEnemyCharacter::CheckTargetLocation, 1.f, true);
		TargetLocation = GetTarget()->GetActorLocation();
		ResetCurrentGridPassbility();
		RPGGameMode->GetPathToDestination(GetActorLocation(), TargetLocation, PathArr, PathCost);
		InitPathStatus();
	}
	else
	{
		DMoveEnd.Broadcast();
	}
}

void ARPGBaseEnemyCharacter::OnCapsuleCollisionEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bAllowCapsuleCollision && bUpdateMovement && Cast<ARPGBaseEnemyCharacter>(OtherActor) && !ShouldIStopMovement())
	{
		bAllowCapsuleCollision = false;
		GetWorldTimerManager().SetTimer(ReactivateCapsuleCollisionTimer, this, &ARPGBaseEnemyCharacter::ReactivateCapsuleCollision, 0.5f);
		RecalculatingPathToTarget();
	}
}

void ARPGBaseEnemyCharacter::ReactivateCapsuleCollision()
{
	bAllowCapsuleCollision = true;
}

void ARPGBaseEnemyCharacter::InitPathStatus()
{
	CurrentPathIdx = -1;
	UpdatePathIndexAndGridPassability();
	NextPoint = FVector(PathArr[0].X, PathArr[0].Y, GetActorLocation().Z);
	NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
}

void ARPGBaseEnemyCharacter::UpdatePathIndexAndGridPassability()
{
	if (CurrentPathIdx + 1 < PathArr.Num())
	{
		RPGGameMode->SetGridToImpassable(PathArr[CurrentPathIdx + 1]);
	}
	if (CurrentPathIdx > 0)
	{
		RPGGameMode->SetGridToPassable(PathArr[CurrentPathIdx - 1]);
	}
	CurrentPathIdx++;
}

void ARPGBaseEnemyCharacter::ResetCurrentGridPassbility()
{
	if (CurrentPathIdx >= 0 && CurrentPathIdx < PathArr.Num()) RPGGameMode->SetGridToPassable(PathArr[CurrentPathIdx]);
	if (CurrentPathIdx > 0) RPGGameMode->SetGridToPassable(PathArr[CurrentPathIdx - 1]);
}

void ARPGBaseEnemyCharacter::UpdateMovementFlowField()
{
	if (ShouldIStopMovement())
	{
		bUpdateMovement = false;
		DMoveEnd.Broadcast();
	}
	else
	{
		const FVector* FlowVector = MySpawner->GetFlowVector(Cast<ACharacter>(GetTarget()), this);
		if (FlowVector)
		{
			AddMovementInput(*FlowVector * DefaultSpeed * GetWorld()->GetDeltaSeconds());
			const FRotator TargetRot = FRotationMatrix::MakeFromX(*FlowVector).Rotator();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 10.f));
		}
		else
		{
			bUpdateMovement = false;
			DMoveEnd.Broadcast();
			MyController->TargetMissed();
		}
	}
}

void ARPGBaseEnemyCharacter::UpdateMovementAStar()
{
	const int32 Dist = FMath::FloorToInt(FVector::Dist(NextPoint, GetActorLocation()));
	if (ShouldIStopMovement() || CurrentPathIdx == PathArr.Num() || Dist > 56)
	{
		if (Dist > 56)
		{
			RecalculatingPathToTarget();
		}
		else
		{
			StopMovement(false);
		}
	}
	else
	{
		if (Dist <= 20)
		{
			UpdatePathIndexAndGridPassability();
			if (CurrentPathIdx < PathArr.Num())
			{
				NextPoint = FVector(PathArr[CurrentPathIdx].X, PathArr[CurrentPathIdx].Y, GetActorLocation().Z);
				NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
			}
		}
		else
		{
			AddMovementInput(NextDirection * DefaultSpeed * GetWorld()->GetDeltaSeconds());
			const FRotator TargetRot = FRotationMatrix::MakeFromX(NextDirection).Rotator();
			SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 10.f));
		}
	}
}

void ARPGBaseEnemyCharacter::RecalculatingPathToTarget()
{
	GetMovementComponent()->StopMovementImmediately();
	ResetCurrentGridPassbility();
	RPGGameMode->ClearEnemiesPathCost(PathCost);
	RPGGameMode->GetPathToDestination(GetActorLocation(), GetTarget()->GetActorLocation(), PathArr, PathCost);
	InitPathStatus();
}

bool ARPGBaseEnemyCharacter::ShouldIStopMovement()
{
	return GetTarget() && FMath::FloorToInt(GetDistanceTo(GetTarget())) <= AttackDistance;
}

void ARPGBaseEnemyCharacter::CheckTargetLocation()
{
	FVector NewTargetLocation = GetTarget()->GetActorLocation();
	if (NewTargetLocation != TargetLocation)
	{
		TargetLocation = NewTargetLocation;
		RecalculatingPathToTarget();
	}
}

void ARPGBaseEnemyCharacter::StopMovement(bool bResetPassbility)
{
	DMoveEnd.Broadcast();
	bUpdateMovement = false;
	GetMovementComponent()->StopMovementImmediately();
	RPGGameMode->ClearEnemiesPathCost(PathCost);
	if (bResetPassbility)
	{
		ResetCurrentGridPassbility();
		PathCost.Empty();
	}
	GetWorldTimerManager().ClearTimer(CheckTargetLocationTimer);
}

/** 공격 */

void ARPGBaseEnemyCharacter::BTTask_Attack()
{
	bIsAttacking = true;
	PlayMeleeAttackEffectMulticast();
}

void ARPGBaseEnemyCharacter::PlayMeleeAttackEffectMulticast_Implementation()
{
	PlayMeleeAttackEffect();
}

void ARPGBaseEnemyCharacter::PlayMeleeAttackEffect()
{
	if (MyAnimInst)
	{
		MyAnimInst->PlayMeleeAttackMontage(HasAuthority());
	}
}

void ARPGBaseEnemyCharacter::Attack()
{
	if (HasAuthority())
	{
		FVector SoundPoint;
		const bool bHitted = EnemyForm->MeleeAttack(this, SoundPoint);
		if (bHitted)
		{
			PlaySoundMulticast(SoundPoint);
		}
	}
}

void ARPGBaseEnemyCharacter::PlaySoundMulticast_Implementation(const FVector_NetQuantize& Location)
{
	if (HasAuthority() == false)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EnemyAssets.MeleeHitSound, Location);
	}
}

void ARPGBaseEnemyCharacter::OnAttackMontageEnded()
{
	DOnAttackEnd.Broadcast();
	bIsAttacking = false;
}

/** 반환 함수 */

void ARPGBaseEnemyCharacter::SetSpawner(AEnemySpawner* Spawner)
{
	MySpawner = Spawner;
	//DHandle = MySpawner->DOnPlayerOut.AddUFunction(this, FName("PlayerOut"));
}

bool ARPGBaseEnemyCharacter::GetSuckedIn() const
{
	return MyController->GetSuckedIn();
}

APawn* ARPGBaseEnemyCharacter::GetTarget() const
{
	return MyController->GetTarget();
}

bool ARPGBaseEnemyCharacter::GetIsInAir() const
{
	return GetMovementComponent()->IsFalling();
}

/** 히트 리액션 */

void ARPGBaseEnemyCharacter::HitReactionMulticast_Implementation()
{
	HitReaction();
}

void ARPGBaseEnemyCharacter::HitReaction()
{
	if (HasAuthority()) MyController->SetIsStunned(true);
	MyAnimInst->PlayHitReactionMontage();
}

void ARPGBaseEnemyCharacter::OnHitReactionEnded()
{
	MyController->SetIsStunned(false);
}

/** 기절 */

void ARPGBaseEnemyCharacter::FalldownMulticast_Implementation()
{
	Falldown();
}

void ARPGBaseEnemyCharacter::Falldown()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(FalldownTimer, this, &ARPGBaseEnemyCharacter::GetupMulticast, 3.f);
		MyController->SetIsFalldown(true);
		if (bUpdateMovement) StopMovement();
	}
	else
	{
		MyAnimInst->PlayDeathMontage();
	}
}

/** 기상 */

void ARPGBaseEnemyCharacter::GetupMulticast_Implementation()
{
	Getup();
}

void ARPGBaseEnemyCharacter::Getup()
{
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(GetupDelayTimer, this, &ARPGBaseEnemyCharacter::GetupDelayEnd, 1.f);
	}
	else
	{
		MyAnimInst->PlayGetupMontage();
	}
}

void ARPGBaseEnemyCharacter::GetupDelayEnd()
{
	MyController->SetIsFalldown(false);
}

/** 커스텀 뎁스 온/오프 */

void ARPGBaseEnemyCharacter::OnRenderCustomDepthEffect(int8 StencilValue)
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

void ARPGBaseEnemyCharacter::OffRenderCustomDepthEffect()
{
	GetMesh()->SetRenderCustomDepth(false);
}

/** 블랙홀 상호작용 */

void ARPGBaseEnemyCharacter::EnableSuckedInToAllClients()
{
	MyController->SetSuckedIn(true);
	if (bUpdateMovement) StopMovement();
	EnableSuckedInMulticast();
}

void ARPGBaseEnemyCharacter::EnableSuckedInMulticast_Implementation()
{
	EnableSuckedIn();
}

void ARPGBaseEnemyCharacter::EnableSuckedIn()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	GetCharacterMovement()->GravityScale = 0.f;
}

void ARPGBaseEnemyCharacter::DisableSuckedInToAllClients()
{
	MyController->SetSuckedIn(false);
	DisableSuckedInMulticast();
}

void ARPGBaseEnemyCharacter::DisableSuckedInMulticast_Implementation()
{
	DisableSuckedIn();
}

void ARPGBaseEnemyCharacter::DisableSuckedIn()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->GravityScale = 1.f;
}

/** 행동 정지, 해제 */

void ARPGBaseEnemyCharacter::StopActionMulticast_Implementation()
{
	StopAction();
}

void ARPGBaseEnemyCharacter::StopAction()
{
	if (HasAuthority())
	{
		MyController->SetIsRestrained(true);
		if (bUpdateMovement) StopMovement();
		GetWorldTimerManager().SetTimer(RestrictionTimer, this, &ARPGBaseEnemyCharacter::ResumeActionMulticast, 5.f);
	}
	else
	{
		GetMesh()->bPauseAnims = true;
	}
}

void ARPGBaseEnemyCharacter::ResumeActionMulticast_Implementation()
{
	ResumeAction();
}

void ARPGBaseEnemyCharacter::ResumeAction()
{
	if (HasAuthority())
	{
		MyController->SetIsRestrained(false);
	}
	else
	{
		GetMesh()->bPauseAnims = false;
	}
}

void ARPGBaseEnemyCharacter::SpawnParticle(UParticleSystem* Particle, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (Particle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, SpawnLocation, SpawnRotation);
	}
}

void ARPGBaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBaseEnemyCharacter, Health);
	DOREPLIFETIME(ARPGBaseEnemyCharacter, MaxHealth);
	DOREPLIFETIME(ARPGBaseEnemyCharacter, Name);
	DOREPLIFETIME(ARPGBaseEnemyCharacter, EnemyAssets);
	DOREPLIFETIME(ARPGBaseEnemyCharacter, bIsActivated);
}

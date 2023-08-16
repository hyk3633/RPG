
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
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

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

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
	WeaponMesh->SetVisibility(false);

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
	}
}

void ARPGBaseEnemyCharacter::ActivateEnemy()
{
	Health = MaxHealth;
	DOnActivate.Broadcast();
	bIsActivated = true;
	SetCollisionActivate();
	if (HasAuthority()) GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->UpdateCharacterExtraCost(LastTimeY, LastTimeX, GetActorLocation());
}

void ARPGBaseEnemyCharacter::OnRep_bIsActivated()
{
	if (bIsActivated)
	{
		GetMesh()->SetVisibility(true);
		WeaponMesh->SetVisibility(true);
		MyAnimInst->CancelMontage();
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
}

void ARPGBaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyAnimInst)
	{
		MyAnimInst->Speed = GetVelocity().Length();
		MyAnimInst->bIsInAir = GetMovementComponent()->IsFalling();
	}

	/*if (HasAuthority())
	{
		if (bUpdateMovement) UpdateMovement();

		CulmulativeTime += DeltaTime;
		if (CulmulativeTime >= 0.1f)
		{
			GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->UpdateCharacterExtraCost(LastTimeY, LastTimeX, GetActorLocation());
			CulmulativeTime = 0.f;
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
	//PLOG(TEXT("%s Enemy damaged : %d"), *DamagedActor->GetName(), FinalDamage);

	ARPGPlayerController* AttackerController = Cast<ARPGPlayerController>(InstigatorController);
	if (AttackerController)
	{
		AttackerController->ReceiveDamageInfo(GetMesh()->GetSocketTransform(FName("DamageSocket")).GetLocation(), FinalDamage);
	}

	SetMatInstDynamicMulticast(GetWorld()->TimeSeconds);

	if (Health != 0)
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
		DOnDeath.Broadcast();
		DisableSuckedInMulticast();
		SetCollisionDeactivate();
		GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->SpawnItems(GetActorLocation());
		bIsActivated = false;
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
	MyAnimInst->PlayDeathMontage();
}

/** 이동 */

void ARPGBaseEnemyCharacter::GetPathToTarget(const FVector& Destination, AEnemySpawner* Spanwer)
{
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetWorldGridManager()->AStarEnemy(GetActorLocation(), Destination, PathArr, Spanwer);

	// 이동 시작
	if (PathArr.Num())
	{
		GetMovementComponent()->StopMovementImmediately();
		PathIdx = 0;
		bUpdateMovement = true;
		NextPoint = FVector(PathArr[0].X, PathArr[0].Y, GetActorLocation().Z);
		NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
	}
}

void ARPGBaseEnemyCharacter::BTTask_Move()
{
	if (HasAuthority() && GetTarget())
	{
		// 이동 경로 받아오기
		const FVector Direction = (GetActorLocation() - GetTarget()->GetActorLocation()).GetSafeNormal();
		const FVector Destination = GetTarget()->GetActorLocation() + (Direction * 150.f);
		//DrawDebugPoint(GetWorld(), Destination, 20, FColor::Turquoise, true);
		GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetActorLocation(), Destination, PathArr);

		// 이동 시작
		if (PathArr.Num())
		{
			GetMovementComponent()->StopMovementImmediately();
			PathIdx = 0;
			bUpdateMovement = true;
			NextPoint = FVector(PathArr[0].X, PathArr[0].Y, GetActorLocation().Z);
			NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
			
			for (int32 i = 0; i < PathArr.Num(); i++)
			{
				DrawDebugPoint(GetWorld(), FVector(PathArr[i].X, PathArr[i].Y, 10.f), 10.f, FColor::Magenta, false, 5.f);
			}
		}
	}
	else
	{
		DMoveEnd.Broadcast();
	}
}

void ARPGBaseEnemyCharacter::UpdateMovement()
{
	if (FVector::Dist(NextPoint, GetActorLocation()) > 20.f)
	{
		AddMovementInput(NextDirection * 20.f * GetWorld()->GetDeltaSeconds());
	}
	else
	{
		PathIdx++;
		if (GetDistanceTo(GetTarget()) <= AttackDistance)
		{
			bUpdateMovement = false;
			GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->UpdateCharacterExtraCost(LastTimeY, LastTimeX, GetActorLocation());
			PathIdx = 0;
			DMoveEnd.Broadcast();
		}
		else
		{
			NextPoint = FVector(PathArr[PathIdx].X, PathArr[PathIdx].Y, GetActorLocation().Z);
			NextDirection = (NextPoint - GetActorLocation()).GetSafeNormal();
		}
	}
}

/** 공격 */

void ARPGBaseEnemyCharacter::BTTask_Attack()
{
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
		MyAnimInst->PlayMeleeAttackMontage();
	}
}

void ARPGBaseEnemyCharacter::Attack()
{
	if (HasAuthority())
	{
		EnemyForm->MeleeAttack(this);
	}
}

void ARPGBaseEnemyCharacter::OnAttackMontageEnded()
{
	DOnAttackEnd.Broadcast();
}

/** 반환 함수 */

bool ARPGBaseEnemyCharacter::GetSuckedIn() const
{
	return MyController->GetSuckedIn();
}

void ARPGBaseEnemyCharacter::SetTarget(APawn* NewTarget)
{
	MyController->SetTarget(NewTarget);
}

APawn* ARPGBaseEnemyCharacter::GetTarget() const
{
	return MyController->GetTarget();
}

bool ARPGBaseEnemyCharacter::GetIsInAir() const
{
	return GetMovementComponent()->IsFalling();
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
		MyController->SetIsFalldown(false);
	}
	else
	{
		MyAnimInst->PlayGetupMontage();
	}
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

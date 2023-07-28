
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "UI/RPGEnemyHealthBarWidget.h"
#include "Player/RPGPlayerController.h"
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
}

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	const int32 FinalDamage = FMath::CeilToInt(CalculateDamage(Damage));
	//PLOG(TEXT("%s Enemy damaged : %d"), *DamagedActor->GetName(), FinalDamage);
	ARPGPlayerController* AttackerController = Cast<ARPGPlayerController>(InstigatorController);
	if (AttackerController)
	{
		AttackerController->ReceiveDamageInfo(GetMesh()->GetSocketTransform(FName("DamageSocket")).GetLocation(), FinalDamage);
	}
	HealthDecrease(FinalDamage);
}

float ARPGBaseEnemyCharacter::CalculateDamage(const float& Damage)
{
	return (Damage * (FMath::RandRange(70, 100))) * (1 - ((DefensivePower * (FMath::RandRange(30, 60) / 10)) / 100));
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

APawn* ARPGBaseEnemyCharacter::GetTarget() const
{
	return MyController->GetTarget();
}

bool ARPGBaseEnemyCharacter::GetIsInAir() const
{
	return GetMovementComponent()->IsFalling();
}

/** 기절 */

void ARPGBaseEnemyCharacter::FalldownToAllClients()
{
	GetWorldTimerManager().SetTimer(FalldownTimer, this, &ARPGBaseEnemyCharacter::GetupToAllClients, 3.f);
	FalldownMulticast();
}

void ARPGBaseEnemyCharacter::FalldownMulticast_Implementation()
{
	Falldown();
}

void ARPGBaseEnemyCharacter::Falldown()
{
	if (HasAuthority())
	{
		MyController->SetIsFalldown(true);
	}
	else
	{
		MyAnimInst->PlayDeathMontage();
	}
}

/** 기상 */

void ARPGBaseEnemyCharacter::GetupToAllClients()
{
	GetupMulticast();
}

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

void ARPGBaseEnemyCharacter::StopActionToAllClients()
{
	StopActionMulticast();
}

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

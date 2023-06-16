
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "UI/RPGEnemyHealthBarWidget.h"
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

	//GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_EnemyBody);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerAttack, ECollisionResponse::ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Widget"));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetAsset(TEXT("WidgetBlueprint'/Game/_Assets/Blueprints/HUD/WBP_EnemyHealthBar.WBP_EnemyHealthBar_C'"));
	if (WidgetAsset.Succeeded()) { HealthBarWidget->SetWidgetClass(WidgetAsset.Class); }
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	HealthBarWidget->SetDrawSize(FVector2D(200.f, 25.f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetVisibility(false);

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

	MyAnimInst = Cast<URPGEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	MyAnimInst->DOnAttack.AddUFunction(this, FName("Attack"));
	MyAnimInst->OnMontageEnded.AddDynamic(this, &ARPGBaseEnemyCharacter::OnAttackMontageEnded);
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

bool ARPGBaseEnemyCharacter::GetIsInAir() const
{
	return GetMovementComponent()->IsFalling();
}

void ARPGBaseEnemyCharacter::BTTask_Attack()
{
	AttackServer();
}

void ARPGBaseEnemyCharacter::OnRenderCustomDepthEffect() const
{
	GetMesh()->SetRenderCustomDepth(true);
}

void ARPGBaseEnemyCharacter::OffRenderCustomDepthEffect() const
{
	GetMesh()->SetRenderCustomDepth(false);
}

void ARPGBaseEnemyCharacter::AnnihilatedByPlayer()
{
	// TODO : ม๏ป็
	MyAnimInst->PlayDeathMontage();
}

void ARPGBaseEnemyCharacter::EnableSuckedIn()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	GetCharacterMovement()->GravityScale = 0.f;
}

void ARPGBaseEnemyCharacter::AttackServer_Implementation()
{
	AttackMulticast();
}

void ARPGBaseEnemyCharacter::AttackMulticast_Implementation()
{
	PlayAttackMontage();
}

void ARPGBaseEnemyCharacter::PlayAttackMontage()
{
	if (MyAnimInst == nullptr) return;
	MyAnimInst->PlayAttackMontage();
}

void ARPGBaseEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MyAnimInst->GetAttackMontage())
	{
		DOnAttackEnd.Broadcast();
	}
}

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	//PLOG(TEXT("%s Enemy damaged : %f"), *DamagedActor->GetName(), Damage);

	HealthDecreaseServer(Damage);
}

void ARPGBaseEnemyCharacter::HealthDecreaseServer_Implementation(const float& Damage)
{
	HealthDecrease(Damage);
}

void ARPGBaseEnemyCharacter::HealthDecrease(const float& Damage)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
	if (Health == 0)
	{
		DOnDeath.Broadcast();
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &ARPGBaseEnemyCharacter::DestroySelf, 5.f, false);
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

	if (Health > 0)
	{
		HealthBarWidget->SetVisibility(true);
		GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ARPGBaseEnemyCharacter::HealthBarVisibilityOff, 60.f);
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerAttack, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
		HealthBarWidget->SetVisibility(false);
		PlayDeathMontage();
	}
}

void ARPGBaseEnemyCharacter::HealthBarVisibilityOff()
{
	HealthBarWidget->SetVisibility(false);
}

void ARPGBaseEnemyCharacter::DeathMulticast_Implementation()
{
	PlayDeathMontage();
}

void ARPGBaseEnemyCharacter::PlayDeathMontage()
{
	MyAnimInst->PlayDeathMontage();
}

void ARPGBaseEnemyCharacter::DestroySelf()
{
	Destroy();
}

void ARPGBaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBaseEnemyCharacter, Health);
}

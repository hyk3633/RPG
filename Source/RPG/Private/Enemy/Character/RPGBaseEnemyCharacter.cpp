
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "UI/RPGEnemyHealthBarWidget.h"
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

	//GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECollisionResponse::ECR_Ignore);
	GetMesh()->CustomDepthStencilValue = 251;

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

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	//PLOG(TEXT("%s Enemy damaged : %f"), *DamagedActor->GetName(), Damage);

	HealthDecrease(Damage);
}

/** 체력 */

void ARPGBaseEnemyCharacter::HealthDecrease(const float& Damage)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
	if (Health == 0)
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerAttack, ECollisionResponse::ECR_Ignore);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
		DOnDeath.Broadcast();
		GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->SpawnItems(GetActorLocation());
		GetWorldTimerManager().SetTimer(DestroyTimer, this, &ARPGBaseEnemyCharacter::DestroySelf, 3.f, false);
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
		EnemyDeath();
	}
}

void ARPGBaseEnemyCharacter::HealthBarVisibilityOff()
{
	HealthBarWidget->SetVisibility(false);
}

/** 죽음 */

void ARPGBaseEnemyCharacter::EnemyDeath()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerAttack, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PlayerProjectile, ECollisionResponse::ECR_Ignore);
	HealthBarWidget->SetVisibility(false);
	MyAnimInst->PlayDeathMontage();
}

void ARPGBaseEnemyCharacter::DestroySelf()
{
	Destroy();
}

/** 공격 */

void ARPGBaseEnemyCharacter::BTTask_Attack()
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

/** 반환 함수 */

bool ARPGBaseEnemyCharacter::GetIsInAir() const
{
	return GetMovementComponent()->IsFalling();
}

/** 기절 */

void ARPGBaseEnemyCharacter::FalldownToAllClients()
{
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

/** 즉사 */

void ARPGBaseEnemyCharacter::InstanceDeath()
{
	HealthDecrease(MaxHealth);
}

/** 블랙홀 상호작용 */

void ARPGBaseEnemyCharacter::EnableSuckedInToAllClients()
{
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

void ARPGBaseEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBaseEnemyCharacter, Health);
}

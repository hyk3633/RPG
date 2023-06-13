
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "../RPG.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void ARPGBaseEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	OnTakeAnyDamage.AddDynamic(this, &ARPGBaseEnemyCharacter::TakeAnyDamage);
}

void ARPGBaseEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();


	
	MyAnimInst = Cast<URPGEnemyAnimInstance>(GetMesh()->GetAnimInstance());
	MyAnimInst->DOnAttack.AddUFunction(this, FName("Attack"));
	MyAnimInst->OnMontageEnded.AddDynamic(this, &ARPGBaseEnemyCharacter::OnAttackMontageEnded);
}

void ARPGBaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	PLOG(TEXT("%s Enemy damaged : %f"), *DamagedActor->GetName(), Damage);
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


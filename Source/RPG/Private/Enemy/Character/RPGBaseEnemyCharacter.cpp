
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "../RPG.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

ARPGBaseEnemyCharacter::ARPGBaseEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//GetMesh()->SetCollisionResponseToChannel(ECC_GroundTrace, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_EnemyBody);
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
	MyAnimInst->DOnAttackTrace.AddUFunction(this, FName("AttackLineTrace"));
	MyAnimInst->OnMontageEnded.AddDynamic(this, &ARPGBaseEnemyCharacter::OnAttackMontageEnded);
}

void ARPGBaseEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGBaseEnemyCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	PLOG(TEXT("Enemy damaged : %f"), Damage);
}

void ARPGBaseEnemyCharacter::BTTask_Attack()
{
	MyAnimInst->PlayAttackMontage();
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

void ARPGBaseEnemyCharacter::AttackLineTrace()
{
	FVector TraceStart = GetActorLocation();
	TraceStart.Z += 50.f;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceStart + GetActorForwardVector() * 150.f, ECC_EnemyAttack);
	DrawDebugLine(GetWorld(), TraceStart, TraceStart + GetActorForwardVector() * 200.f, FColor::Red, false, 3.f, 0U, 2.f);

	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), 50.f, Controller, this, UDamageType::StaticClass());
	}
}

void ARPGBaseEnemyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == MyAnimInst->GetAttackMontage())
	{
		DOnAttackEnd.Broadcast();
	}
}


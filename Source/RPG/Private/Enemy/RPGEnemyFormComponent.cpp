
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "Projectile/RPGBaseProjectile.h"
#include "GameSystem/ProjectilePoolerComponent.h"
#include "../RPG.h"
#include "../RPGGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"

URPGEnemyFormComponent::URPGEnemyFormComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ProjectilePooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Projectile Pooler"));
}

void URPGEnemyFormComponent::InitEnemyFormComponent(EEnemyType Type)
{
	GetEnemyInfoAndInitialize(Type);
	GetEnemyAssetsAndInitialize(Type);
}

void URPGEnemyFormComponent::GetEnemyInfoAndInitialize(EEnemyType Type)
{
	const FEnemyInfo& NewEnemyInfo = *GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetEnemyInfo(Type);
	EnemyInfo = NewEnemyInfo;
}

void URPGEnemyFormComponent::GetEnemyAssetsAndInitialize(EEnemyType Type)
{
	FEnemyAssets* NewEnemyAssets = GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetEnemyAssets(Type);
	if (NewEnemyAssets)
	{
		EnemyAssets = *NewEnemyAssets;

		if (EnemyAssets.WeaponMesh_Skeletal) bIsWeaponed = true;
		if (EnemyAssets.ProjectileType != EProjectileType::EPT_None) CreateProjectilePooler();
	}
}

void URPGEnemyFormComponent::CreateProjectilePooler()
{
	ProjectilePooler->CreatePool(EnemyAssets.ProjectileClass, 20, EnemyAssets.ProjectileType);
}

ARPGBaseEnemyCharacter* URPGEnemyFormComponent::CreateNewEnemy()
{
	ARPGBaseEnemyCharacter* NewEnemy = nullptr;
	if (EnemyAssets.AttackType == EEnemyAttackType::EEAT_Melee)
	{
		NewEnemy = GetWorld()->SpawnActorDeferred<ARPGBaseEnemyCharacter>(ARPGBaseEnemyCharacter::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
	}
	else
	{
		NewEnemy = GetWorld()->SpawnActorDeferred<ARPGRangedEnemyCharacter>(ARPGRangedEnemyCharacter::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
	}

	if (NewEnemy)
	{
		InitEnemy(NewEnemy);
		NewEnemy->SetEnemyAssets(EnemyAssets);
		NewEnemy->FinishSpawning(FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
		return NewEnemy;
	}
	else return nullptr;
}

void URPGEnemyFormComponent::InitEnemy(ARPGBaseEnemyCharacter* SpawnedEnemy)
{
	SpawnedEnemy->EnemyForm = this;

	SpawnedEnemy->Name = EnemyInfo.Name;
	SpawnedEnemy->MaxHealth = EnemyInfo.MaxHealth;
	SpawnedEnemy->StrikingPower = EnemyInfo.StrikingPower;
	SpawnedEnemy->DefensivePower = EnemyInfo.DefensivePower;
	SpawnedEnemy->Exp = EnemyInfo.Exp;

	SpawnedEnemy->Health = EnemyInfo.MaxHealth;
}

void URPGEnemyFormComponent::MeleeAttack(ARPGBaseEnemyCharacter* Attacker)
{
	FVector TraceStart = Attacker->GetActorLocation();
	TraceStart.Z += 50.f;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceStart + Attacker->GetActorForwardVector() * 150.f, ECC_EnemyAttack);
	DrawDebugLine(GetWorld(), TraceStart, TraceStart + Attacker->GetActorForwardVector() * 200.f, FColor::Red, false, 3.f, 0U, 2.f);

	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), EnemyInfo.StrikingPower, Attacker->GetController(), Attacker, UDamageType::StaticClass());
	}
}

void URPGEnemyFormComponent::RangedAttack(ARPGBaseEnemyCharacter* Attacker)
{
	if (EnemyAssets.ProjectileClass == nullptr) return;

	GetSocketLocationAndSpawn(Attacker);
}

void URPGEnemyFormComponent::GetSocketLocationAndSpawn(ARPGBaseEnemyCharacter* Attacker)
{
	const FVector TraceStart = (bIsWeaponed ? Attacker->WeaponMesh : Attacker->GetMesh())->GetSocketTransform(FName("Muzzle_Socket")).GetLocation();
	FVector TraceEnd = TraceStart;

	if (Attacker->GetTarget())
	{
		TraceEnd += (Attacker->GetTarget()->GetActorLocation() - TraceStart);
	}
	else return;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_EnemyAttack);
	if (HitResult.bBlockingHit)
	{
		FRotator FireRotation = (HitResult.ImpactPoint - TraceStart).Rotation();
		FireRotation.Pitch = 0.f;

		SpawnProjectile(Attacker, TraceStart, FireRotation);
	}
}

void URPGEnemyFormComponent::SpawnProjectile(ARPGBaseEnemyCharacter* Attacker, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (ProjectilePooler == nullptr) return;
	
	ARPGBaseProjectile* Projectile = ProjectilePooler->GetPooledProjectile(Attacker, EnemyInfo.StrikingPower);
	if (Projectile)
	{
		Projectile->SetActorLocation(SpawnLocation);
		Projectile->SetActorRotation(SpawnRotation);
		Projectile->ActivateProjectileToAllClients();
	}
}

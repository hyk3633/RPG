
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Enemy/Boss/RPGBossEnemyCharacter.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/RPGEnemyAnimInstance.h"
#include "Projectile/RPGBaseProjectile.h"
#include "GameSystem/ProjectilePoolerComponent.h"
#include "../RPG.h"
#include "../RPGGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "DamageType/DamageTypeStunAndPush.h"

URPGEnemyFormComponent::URPGEnemyFormComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ProjectilePooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Projectile Pooler"));
}

void URPGEnemyFormComponent::InitEnemyFormComponent(EEnemyType Type)
{
	EnemyType = Type;
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
	else if(EnemyAssets.AttackType == EEnemyAttackType::EEAT_Boss)
	{
		NewEnemy = GetWorld()->SpawnActorDeferred<ARPGBossEnemyCharacter>(ARPGBossEnemyCharacter::StaticClass(), FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
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
	SpawnedEnemy->Health = EnemyInfo.MaxHealth;
	SpawnedEnemy->DefensivePower = EnemyInfo.DefensivePower;
	SpawnedEnemy->AttackDistance = EnemyInfo.AttackDistance;
	SpawnedEnemy->DetectDistance = EnemyInfo.DetectDistance;
	SpawnedEnemy->EnemyType = EnemyType;
	SpawnedEnemy->AttackType = EnemyAssets.AttackType;
}

void URPGEnemyFormComponent::MeleeAttack(ARPGBaseEnemyCharacter* Attacker)
{
	const FVector TraceStart = Attacker->GetMesh()->GetSocketTransform(FName("Melee_Socket")).GetLocation();

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceStart + Attacker->GetActorForwardVector() * 150.f, ECC_EnemyAttack);
	DrawDebugLine(GetWorld(), TraceStart, TraceStart + Attacker->GetActorForwardVector() * 200.f, FColor::Red, false, 3.f, 0U, 2.f);

	if (HitResult.bBlockingHit)
	{
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), EnemyInfo.StrikingPower, Attacker->GetController(), Attacker, nullptr);
	}
}

void URPGEnemyFormComponent::RangedAttack(ARPGBaseEnemyCharacter* Attacker, APawn* HomingTarget)
{
	if (EnemyAssets.ProjectileClass == nullptr) return;

	GetSocketLocationAndSpawn(Attacker, HomingTarget);
}

void URPGEnemyFormComponent::GetSocketLocationAndSpawn(ARPGBaseEnemyCharacter* Attacker, APawn* HomingTarget)
{
	const FVector TraceStart = (bIsWeaponed ? Attacker->WeaponMesh : Attacker->GetMesh())->GetSocketTransform(FName("Muzzle_Socket")).GetLocation();
	FVector TraceEnd = TraceStart;

	DrawDebugPoint(GetWorld(), TraceStart, 20, FColor::Blue, true);

	if (IsValid(HomingTarget))
	{
		TraceEnd += (HomingTarget->GetActorLocation() - TraceStart);
	}

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_EnemyAttack);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, true);
	if (HitResult.bBlockingHit)
	{
		FRotator FireRotation = (HitResult.ImpactPoint - TraceStart).Rotation();
		FireRotation.Pitch = 0.f;

		SpawnProjectile(Attacker, TraceStart, FireRotation, HomingTarget);
	}
}

void URPGEnemyFormComponent::SpawnProjectile(ARPGBaseEnemyCharacter* Attacker, const FVector& SpawnLocation, const FRotator& SpawnRotation, APawn* HomingTarget)
{
	if (ProjectilePooler == nullptr) return;
	
	ARPGBaseProjectile* Projectile = ProjectilePooler->GetPooledProjectile(Attacker, EnemyInfo.StrikingPower);
	if (Projectile)
	{
		Projectile->SetActorLocation(SpawnLocation);
		Projectile->SetActorRotation(SpawnRotation);
		Projectile->SetHomingTarget(Cast<ACharacter>(HomingTarget));
		Projectile->ActivateProjectileToAllClients();
	}
}

void URPGEnemyFormComponent::StraightMultiAttack(ARPGBaseEnemyCharacter* Attacker, const FVector& LocationToAttack, TArray<FVector>& ImpactLocation, TArray<FRotator>& ImpactRotation)
{
	const FVector TraceStart = Attacker->GetMesh()->GetSocketTransform(FName("Melee_Socket")).GetLocation();

	TArray<FHitResult> HitResults;
	GetWorld()->LineTraceMultiByChannel(HitResults, TraceStart, LocationToAttack, ECC_EnemyAttack);

	for (FHitResult Hit : HitResults)
	{
		if (Hit.bBlockingHit)
		{
			UGameplayStatics::ApplyDamage(Hit.GetActor(), EnemyInfo.StrikingPower, Attacker->GetController(), Attacker, nullptr);
			ImpactLocation.Add(Hit.ImpactPoint);
			ImpactRotation.Add(Hit.ImpactNormal.Rotation());
		}
	}
}

void URPGEnemyFormComponent::SphericalRangeAttack(ARPGBaseEnemyCharacter* Attacker, const int32& Radius, TArray<FVector>& ImpactLocation, TArray<FRotator>& ImpactRotation)
{
	TArray<FHitResult> HitResults;
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		Attacker->GetActorLocation(),
		Attacker->GetActorLocation(),
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_EnemyAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	for (FHitResult Hit : HitResults)
	{
		if (Hit.bBlockingHit)
		{
			UGameplayStatics::ApplyDamage(Hit.GetActor(), EnemyInfo.StrikingPower, Attacker->GetController(), Attacker, UDamageTypeStunAndPush::StaticClass());
			ImpactLocation.Add(Hit.ImpactPoint);
			ImpactRotation.Add(Hit.ImpactNormal.Rotation());
		}
	}
}

void URPGEnemyFormComponent::RectangularRangeAttack(ARPGBaseEnemyCharacter* Attacker, const int32& Length, TArray<FVector>& ImpactLocation, TArray<FRotator>& ImpactRotation)
{
	TArray<FHitResult> HitResults;
	FVector Location = Attacker->GetActorForwardVector() * 450;
	Location.Z = 10;
	UKismetSystemLibrary::BoxTraceMulti
	(
		this,
		Location,
		Location,
		FVector(600, 200, 10),
		FRotator::ZeroRotator,
		UEngineTypes::ConvertToTraceType(ECC_EnemyAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	for (FHitResult Hit : HitResults)
	{
		if (Hit.bBlockingHit)
		{
			UGameplayStatics::ApplyDamage(Hit.GetActor(), EnemyInfo.StrikingPower, Attacker->GetController(), Attacker, UDamageTypeStunAndPush::StaticClass());
			ImpactLocation.Add(Hit.ImpactPoint);
			ImpactRotation.Add(Hit.ImpactNormal.Rotation());
		}
	}
}

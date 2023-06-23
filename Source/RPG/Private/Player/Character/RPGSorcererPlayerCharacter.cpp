

#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Projectile/RPGBaseProjectile.h"
#include "Projectile/RPGSpeedDownProjectile.h"
#include "Player/CharacterAbility/RPGBlackhole.h"
#include "../RPG.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemlibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

#include "DrawDebugHelpers.h"

ARPGSorcererPlayerCharacter::ARPGSorcererPlayerCharacter()
{
	MaxCombo = 4;
}

void ARPGSorcererPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFloatCharacter)
	{
		FVector Loc = GetActorLocation();
		Loc.Z += 1.6f;
		SetActorLocation(Loc);
	}
}

void ARPGSorcererPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGSorcererPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetRPGAnimInstance())
	{
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FireSpeedDownBall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorliteFall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorShower"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FloatingCharacter"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SummonBlackhole"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("BlackholeEnd"));
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// TODO : R 스킬도 인트로 애니메이션 만들기
	// R 스킬 제외한 스킬만 인트로 애니메이션 재생
	if (KeyType != EPressedKey::EPK_R)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
	}
	RPGAnimInstance->AimingPoseOn();
	if (IsLocallyControlled())
	{
		bAiming = true;
		AimCursor->SetVisibility(true);
		TargetingCompOn();
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	RPGAnimInstance->PlayAbilityMontageOfKey(true);
	RPGAnimInstance->AimingPoseOff();
	if (IsLocallyControlled())
	{
		TargetingCompOff();
	}
}

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	if (IsLocallyControlled() && PrimaryPorjectile)
	{
		SpawnNormalProjectileServer();
	}
}

void ARPGSorcererPlayerCharacter::SpawnNormalProjectileServer_Implementation()
{
	SpawnNormalProjectile();
}

void ARPGSorcererPlayerCharacter::SpawnNormalProjectile()
{
	FVector SpawnPoint;
	if (GetCurrentCombo() % 2 == 0)
	{
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_R")).GetLocation();
	}
	else
	{
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L")).GetLocation();
	}

	FRotator SpawnDirection;
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(TargetingHitResult.GetActor());
	if (Enemy && GetDistanceTo(Enemy) <= AttackRange)
	{
		SpawnDirection = (Enemy->GetActorLocation() - SpawnPoint).Rotation();
	}
	else
	{
		SpawnDirection = (FVector(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, SpawnPoint.Z) - SpawnPoint).Rotation();
	}

	ARPGBaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARPGBaseProjectile>(PrimaryPorjectile, FTransform(SpawnDirection, SpawnPoint), this, this);
	if (Projectile)
	{
		Projectile->SetProjectileData(FProjectileData(true, 50, 1, 1000, 32));
		Projectile->FinishSpawning(FTransform(SpawnDirection, SpawnPoint));
	}
}

void ARPGSorcererPlayerCharacter::FireSpeedDownBall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_Q_FireRestrictionBall) return;

	if (IsLocallyControlled())
	{
		FireSpeedDownBallServer();
	}
}

void ARPGSorcererPlayerCharacter::FireSpeedDownBallServer_Implementation()
{
	SpawnSpeedDownProjectile();
}

void ARPGSorcererPlayerCharacter::SpawnSpeedDownProjectile()
{
	FVector SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L")).GetLocation();
	FRotator SpawnDirection = (TargetingHitResult.GetActor()->GetActorLocation() - SpawnPoint).Rotation();
	ARPGSpeedDownProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARPGSpeedDownProjectile>(SpeedDownPorjectile, FTransform(SpawnDirection.Add(60.f, 0.f, 0.f), SpawnPoint), this, this);
	if (Projectile)
	{
		Projectile->SetProjectileData(FProjectileData(true, 30, 2, 1000, 32, true, 300));
		ACharacter* TargetCha = Cast<ACharacter>(TargetingHitResult.GetActor());
		if (TargetCha) Projectile->SetHomingTarget(TargetCha);
		Projectile->FinishSpawning(FTransform(SpawnDirection, SpawnPoint));
	}
}

void ARPGSorcererPlayerCharacter::MeteorliteFall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_W_MeteorliteFall) return;

	if (IsLocallyControlled())
	{
		MeteorliteFallServer();
	}
}

void ARPGSorcererPlayerCharacter::MeteorliteFallServer_Implementation()
{
	SpawnMeteorProjectile();
}

void ARPGSorcererPlayerCharacter::SpawnMeteorProjectile()
{
	const FVector SpawnPoint = GetActorLocation() + (GetActorUpVector() * 300.f);
	const FTransform SpanwTransform((TargetingHitResult.ImpactPoint - SpawnPoint).Rotation(), SpawnPoint);
	if (MeteorlitePortalParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorlitePortalParticle, SpanwTransform);
	}
	ARPGBaseProjectile* Projectile = GetWorld()->SpawnActorDeferred<ARPGBaseProjectile>(MeteorlitePorjectile, SpanwTransform, this, this);
	if (Projectile)
	{
		Projectile->SetProjectileData(FProjectileData(true, 600, 1, 1000, 70, true, 600));
		Projectile->FinishSpawning(SpanwTransform);
	}
}

void ARPGSorcererPlayerCharacter::MeteorShower(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_E_MeteorShower) return;

	if (IsLocallyControlled())
	{
		MeteorShowerServer();
	}
}

void ARPGSorcererPlayerCharacter::MeteorShowerServer_Implementation()
{
	SphereTraceLocation = TargetingHitResult.ImpactPoint;
	SpawnMeteorPortalParticleMulticast();
	GetWorldTimerManager().SetTimer(MeteorShowerTimer, this, &ARPGSorcererPlayerCharacter::MeteorShowerOn, 0.5f, false);
}

void ARPGSorcererPlayerCharacter::SpawnMeteorPortalParticleMulticast_Implementation()
{
	if (HasAuthority()) return;
	if (MeteorPortalParticle)
	{
		FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
		PSpawnLoc.Z = 500.f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorPortalParticle, PSpawnLoc, FRotator::ZeroRotator);
	}
}

void ARPGSorcererPlayerCharacter::MeteorShowerOn()
{
	MeteorDamageCount = 0;
	GetWorldTimerManager().SetTimer(MeteorDamageTimer, this, &ARPGSorcererPlayerCharacter::ApplyMeteorDamage, 0.3f, true, 0.2f);
	SpawnMeteorShowerParticleMulticast();
}

void ARPGSorcererPlayerCharacter::ApplyMeteorDamage()
{
	if (MeteorDamageCount == 5)
	{
		GetWorldTimerManager().ClearTimer(MeteorDamageTimer);
	}
	TArray<FHitResult> Hits;
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		SphereTraceLocation,
		SphereTraceLocation,
		500.f,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::Persistent,
		Hits,
		true
	);
	for (FHitResult Hit : Hits)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy == nullptr) continue;
		UGameplayStatics::ApplyDamage(Enemy, 30.f, GetController(), this, UDamageType::StaticClass());
	}
	MeteorDamageCount++;
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticleMulticast_Implementation()
{
	SpawnMeteorShowerParticle();
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle()
{
	if (HasAuthority()) return;
	if (MeteorShowerParticle == nullptr) return;
	FVector PSpawnLoc = SphereTraceLocation;
	PSpawnLoc.Z = 500.f;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator, true);
}

void ARPGSorcererPlayerCharacter::FloatingCharacter(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_FloatingCharacter) return;
	
	if (IsLocallyControlled())
	{
		FloatingCharacterServer();
	}
}

void ARPGSorcererPlayerCharacter::FloatingCharacterServer_Implementation()
{
	bFloatCharacter = true;
	SetMovementModeToFlyMulticast();
	GetWorldTimerManager().SetTimer(FloatingTimer, this, &ARPGSorcererPlayerCharacter::StopFloatingCharacter, 2.f, false);
}

void ARPGSorcererPlayerCharacter::SetMovementModeToFlyMulticast_Implementation()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void ARPGSorcererPlayerCharacter::StopFloatingCharacter()
{
	bFloatCharacter = false;
}

void ARPGSorcererPlayerCharacter::SummonBlackhole(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_SummonBlackhole) return;

	if (IsLocallyControlled())
	{
		SpawnBlackholeServer();
	}
}

void ARPGSorcererPlayerCharacter::SpawnBlackholeServer_Implementation()
{
	SpawnBlackholeMulticast();
}

void ARPGSorcererPlayerCharacter::SpawnBlackholeMulticast_Implementation()
{
	SpawnBlackhole();
}

void ARPGSorcererPlayerCharacter::SpawnBlackhole()
{
	if (HasAuthority())
	{
		if (BlackholeClass == nullptr) return;
		FVector Location = TargetingHitResult.ImpactPoint;
		Location.Z += 300.f;
		GetWorld()->SpawnActor<ARPGBlackhole>(BlackholeClass, Location, FRotator::ZeroRotator);
	}
	else
	{
		FVector Location = TargetingHitResult.ImpactPoint;
		Location.Z += 300.f;
		SpawnParticle(BlackholeParticle, Location);
	}
}

void ARPGSorcererPlayerCharacter::BlackholeEnd(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_BlackholeEnd) return;

	if (IsLocallyControlled())
	{
		SetMovementModeToWalkServer();
	}
}

void ARPGSorcererPlayerCharacter::SetMovementModeToWalkServer_Implementation()
{
	SetMovementModeToWalkMulticast();
}

void ARPGSorcererPlayerCharacter::SetMovementModeToWalkMulticast_Implementation()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void ARPGSorcererPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGSorcererPlayerCharacter, SphereTraceLocation);
	DOREPLIFETIME(ARPGSorcererPlayerCharacter, bFloatCharacter);
}

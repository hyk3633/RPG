

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
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FloatACharacter"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SummonBlackhole"));
	}
	if (MeteorShowerParticleComp)
		MeteorShowerParticleComp->Deactivate();
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
	bAiming = true;
	if (IsLocallyControlled())
	{
		AimCursor->SetVisibility(true);
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	if (TargetingHitResult.bBlockingHit == false) return;
	if (HasAuthority()) return;

	RPGAnimInstance->PlayAbilityMontageOfKey(true);
	RPGAnimInstance->AimingPoseOff();
	if (RPGAnimInstance->GetCurrentState() == EPressedKey::EPK_R)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	}
}

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	if (IsLocallyControlled() && PrimaryPorjectile)
	{

	}

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
		// TODO : 범위 내 적들에게 확실하게 데미지 주는 방식으로 재설계
		MeteorShowerServer();
	}
}

void ARPGSorcererPlayerCharacter::MeteorShowerServer_Implementation()
{
	SpawnMeteorPortalParticleMulticast();
	GetWorldTimerManager().SetTimer(MeteorShowerTimer, this, &ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle, 0.5f, false);
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

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticleMulticast_Implementation()
{
	if (MeteorShowerParticle == nullptr) return;

	FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
	PSpawnLoc.Z = 500.f;
	if (IsLocallyControlled())
	{
		MeteorShowerParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator, false);
		if (MeteorShowerParticleComp)
		{
			MeteorShowerParticleComp->Activate();
			MeteorShowerParticleComp->OnParticleCollide.AddDynamic(this, &ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide);
		}
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator, false, EPSCPoolMethod::None, false);
	}
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle()
{
	SpawnMeteorShowerParticleMulticast();
}

void ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	ApplyMeteorRadialDamageServer(Location);
}

void ARPGSorcererPlayerCharacter::ApplyMeteorRadialDamageServer_Implementation(const FVector_NetQuantize& Location)
{
	TArray<AActor*> IgnoreActors;
	for (AActor* Actor : TActorRange<ARPGBasePlayerCharacter>(GetWorld()))
		IgnoreActors.Add(Actor);
	UGameplayStatics::ApplyRadialDamage(this, 150.f, Location, 150.f, UDamageType::StaticClass(), IgnoreActors, this, GetController());
}

void ARPGSorcererPlayerCharacter::FloatACharacter(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_FloatACharacter) return;
	
	bFloatCharacter = true;
	GetWorldTimerManager().SetTimer(FloatingTimer, this, &ARPGSorcererPlayerCharacter::DeactivateFloatingCharacter, 2.f, false);
}

void ARPGSorcererPlayerCharacter::DeactivateFloatingCharacter()
{
	bFloatCharacter = false;
}

void ARPGSorcererPlayerCharacter::SummonBlackhole(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_BlackholeOn) return;

	if (BlackholeClass)
	{
		FVector SpawnLocation = TargetingHitResult.ImpactPoint;
		SpawnLocation.Z += 300.f;
		GetWorld()->SpawnActor<ARPGBlackhole>(BlackholeClass, SpawnLocation, FRotator::ZeroRotator);
	}
}

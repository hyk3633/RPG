

#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "Player/AnimInstance/RPGSorcererAnimInstance.h"
#include "Player/CharacterAbility/RPGBlackhole.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Projectile/RPGBaseProjectile.h"
#include "GameSystem/ProjectilePoolerComponent.h"
#include "../RPG.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemlibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"
#include "Materials/MaterialInstanceConstant.h"

#include "DrawDebugHelpers.h"

ARPGSorcererPlayerCharacter::ARPGSorcererPlayerCharacter()
{
	MaxCombo = 4;

	PrimaryProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Primary Projectile Pooler"));

	RestrictionProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Restriction Projectile Pooler"));

	MeteorliteProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Meteorlite Projectile Pooler"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> MaterialAsset(TEXT("MaterialInstanceConstant'/Game/_Assets/Materials/Targeting/MI_SorcererTargeting.MI_SorcererTargeting'"));
	if (MaterialAsset.Succeeded()) { AimCursor->SetMaterial(0, MaterialAsset.Object); }
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

	SetAbilityCooldownTime(10, 12, 15, 30);
	SetAbilityManaUsage(30, 50, 100, 150);
	SetSkillPowerCorrectionValues(0.f, 2.f, 1.3f, -0.5f);
}

void ARPGSorcererPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetRPGAnimInstance())
	{
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FireRestrictionBall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorliteFall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorShower"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FloatingCharacter"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SummonBlackhole"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("BlackholeEnd"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("BlackholeBeamOn"));
	}

	if (HasAuthority())
	{
		PrimaryProjPooler->CreatePool(ARPGBaseProjectile::StaticClass(), 50, EProjectileType::EPT_Sorcerer_Primary);
		RestrictionProjPooler->CreatePool(ARPGBaseProjectile::StaticClass(), 5, EProjectileType::EPT_Sorcerer_Restriction);
		MeteorliteProjPooler->CreatePool(ARPGBaseProjectile::StaticClass(), 5, EProjectileType::EPT_Sorcerer_Meteorlite);
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	if (HasAuthority()) return;

	// R 스킬 제외한 스킬만 인트로 애니메이션 재생
	URPGSorcererAnimInstance* SAnimInstance = Cast<URPGSorcererAnimInstance>(RPGAnimInstance);
	if (SAnimInstance)
	{
		if (KeyType != EPressedKey::EPK_R) SAnimInstance->PlayAbilityIntroMontage();
		SAnimInstance->AimingPoseOn();
	}

	if (IsLocallyControlled())
	{
		bAiming = true;
		AimCursor->SetVisibility(true);
		if (KeyType == EPressedKey::EPK_Q)
		{
			TargetingCompOn(30);
		}
		else if (KeyType == EPressedKey::EPK_W)
		{
			TargetingCompOn(350);
		}
		else
		{
			TargetingCompOn(500);
		}
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityAfterTargeting()
{
	if (TargetingHitResult.bBlockingHit == false) return;
	Super::CastAbilityAfterTargeting();
	if (HasAuthority()) return;

	RPGAnimInstance->PlayAbilityMontageOfKey();
	URPGSorcererAnimInstance* SAnimInstance = Cast<URPGSorcererAnimInstance>(RPGAnimInstance);
	if (SAnimInstance) SAnimInstance->AimingPoseOff();
	if (IsLocallyControlled()) TargetingCompOff();
}

void ARPGSorcererPlayerCharacter::OnAbilityEnded(EPressedKey KeyType)
{
	if (IsLocallyControlled())
	{
		switch (KeyType)
		{
		case EPressedKey::EPK_Q:
			AbilityActiveBitOff(EPressedKey::EPK_Q);
			AbilityCooldownStartServer(EPressedKey::EPK_Q);
			break;
		case EPressedKey::EPK_W:
			AbilityActiveBitOff(EPressedKey::EPK_W);
			AbilityCooldownStartServer(EPressedKey::EPK_W);
			break;
		case EPressedKey::EPK_R:
			AbilityActiveBitOff(EPressedKey::EPK_R);
			AbilityCooldownStartServer(EPressedKey::EPK_R);
			break;
		}
	}
}

void ARPGSorcererPlayerCharacter::CancelAbility()
{
	Super::CancelAbility();

	if (IsLocallyControlled())
	{
		AimingPoseOffServer();
	}
}

void ARPGSorcererPlayerCharacter::AimingPoseOffServer_Implementation()
{
	AimingPoseOffMulticast();
}

void ARPGSorcererPlayerCharacter::AimingPoseOffMulticast_Implementation()
{
	if (HasAuthority()) return;

	URPGSorcererAnimInstance* SAnimInstance = Cast<URPGSorcererAnimInstance>(RPGAnimInstance);
	if (SAnimInstance) SAnimInstance->AimingPoseOff();
}

/** --------------------------- 일반 공격 --------------------------- */

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	if (IsLocallyControlled())
	{
		SpawnNormalProjectileServer(GetSocketLocation());
	}
}

FVector ARPGSorcererPlayerCharacter::GetSocketLocation(FName SocketName)
{
	if (SocketName.IsNone())
	{
		if (GetCurrentCombo() % 2 == 0)
		{
			return GetMesh()->GetSocketTransform(FName("Muzzle_R2")).GetLocation();
		}
		else
		{
			return GetMesh()->GetSocketTransform(FName("Muzzle_L2")).GetLocation();
		}
	}

	return GetMesh()->GetSocketTransform(SocketName).GetLocation();
}

void ARPGSorcererPlayerCharacter::SpawnNormalProjectileServer_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	SpawnNormalProjectile(SpawnLocation);
}

void ARPGSorcererPlayerCharacter::SpawnNormalProjectile(const FVector& SpawnLocation)
{
	FRotator SpawnDirection;
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(TargetingHitResult.GetActor());
	if (Enemy && GetDistanceTo(Enemy) <= AttackRange)
	{
		SpawnDirection = (Enemy->GetActorLocation() - SpawnLocation).Rotation();
	}
	else
	{
		SpawnDirection = (FVector(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, SpawnLocation.Z) - SpawnLocation).Rotation();
	}

	SpawnProjectile(EProjectileType::EPT_Sorcerer_Primary, SpawnLocation, SpawnDirection);
}

void ARPGSorcererPlayerCharacter::SpawnProjectile(const EProjectileType Type, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	ARPGBaseProjectile* Projectile = nullptr;
	if (Type == EProjectileType::EPT_Sorcerer_Primary)
	{
		Projectile = PrimaryProjPooler->GetPooledProjectile(this, GetStrikingPower());

	}
	else if (Type == EProjectileType::EPT_Sorcerer_Restriction)
	{
		Projectile = RestrictionProjPooler->GetPooledProjectile(this, GetSkillPower(EPressedKey::EPK_Q));
	}
	else if (Type == EProjectileType::EPT_Sorcerer_Meteorlite)
	{
		Projectile = MeteorliteProjPooler->GetPooledProjectile(this, GetSkillPower(EPressedKey::EPK_W));
	}

	if (Projectile)
	{
		Projectile->SetActorLocation(SpawnLocation);
		Projectile->SetActorRotation(SpawnRotation);
		Projectile->ActivateProjectileToAllClients();
	}
}

/** --------------------------- Q 스킬 --------------------------- */

void ARPGSorcererPlayerCharacter::FireRestrictionBall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_Q_FireRestrictionBall) return;

	if (IsLocallyControlled())
	{
		FireRestrictionBallServer(GetSocketLocation(FName("Muzzle_L")));
	}
}

void ARPGSorcererPlayerCharacter::FireRestrictionBallServer_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	SpawnRestrictionProjectile(SpawnLocation);
}

void ARPGSorcererPlayerCharacter::SpawnRestrictionProjectile(const FVector& SpawnLocation)
{
	FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnLocation).Rotation();

	SpawnProjectile(EProjectileType::EPT_Sorcerer_Restriction, SpawnLocation, SpawnDirection);
}

/** --------------------------- W 스킬 --------------------------- */

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
	const FVector SpawnPoint = GetActorLocation() + (GetActorUpVector() * 500.f);
	const FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnPoint).Rotation();
	
	SpawnMeteorlietPortalParticleMulticast(SpawnPoint, SpawnDirection);

	SpawnProjectile(EProjectileType::EPT_Sorcerer_Meteorlite, SpawnPoint, SpawnDirection);
}

void ARPGSorcererPlayerCharacter::SpawnMeteorlietPortalParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation, const FRotator& SpawnRotation)
{
	if (HasAuthority() == false)
	{
		SpawnParticle(MeteorlitePortalParticle, SpawnLocation, SpawnRotation);
	}
}

/** --------------------------- E 스킬 --------------------------- */

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
	GetWorldTimerManager().SetTimer(MeteorFireTimer, this, &ARPGSorcererPlayerCharacter::SpawnMeteorShowerFireSoundMulticast, 0.3f, true, 0.3f);
}

void ARPGSorcererPlayerCharacter::SpawnMeteorPortalParticleMulticast_Implementation()
{
	if (HasAuthority()) return;
	if (MeteorPortalParticle)
	{
		FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
		PSpawnLoc.Z = 500.f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorPortalParticle, PSpawnLoc, FRotator::ZeroRotator);
		if (MeteorShowerPortalSound) UGameplayStatics::PlaySoundAtLocation(this, MeteorShowerPortalSound, PSpawnLoc, 0.3f);
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
		GetWorldTimerManager().ClearTimer(MeteorFireTimer);
		AbilityActiveBitOffClient(EPressedKey::EPK_E);
		AbilityCooldownStart(EPressedKey::EPK_E);
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
	const float Damage = GetSkillPower(EPressedKey::EPK_E);
	for (FHitResult Hit : Hits)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy == nullptr || Enemy->GetIsActivated() == false) continue;
		ApplyDamageToEnemy(Enemy, Damage);
	}
	SpawnMeteorShowerImpactSoundMulticast();
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

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerFireSoundMulticast_Implementation()
{
	if (HasAuthority()) return;
	if (MeteorShowerFireSound) UGameplayStatics::PlaySoundAtLocation(this, MeteorShowerFireSound, SphereTraceLocation, 2.f);
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerImpactSoundMulticast_Implementation()
{
	if (HasAuthority()) return;
	if (MeteorShowerImpactSound) UGameplayStatics::PlaySoundAtLocation(this, MeteorShowerImpactSound, SphereTraceLocation, 2.f);
}

/** --------------------------- R 스킬 --------------------------- */

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
		ARPGBlackhole* Blackhole = GetWorld()->SpawnActor<ARPGBlackhole>(BlackholeClass, Location, FRotator::ZeroRotator);
		if (Blackhole) Blackhole->SetDamage(GetSkillPower(EPressedKey::EPK_R));
	}
	else
	{
		FVector Location = TargetingHitResult.ImpactPoint;
		Location.Z += 300.f;
		SpawnParticle(BlackholeParticle, Location, FRotator(0, 0, 180));
	}
}

void ARPGSorcererPlayerCharacter::BlackholeBeamOn(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_BlackholeBeam) return;

	if (IsLocallyControlled())
	{
		ActivateBlackholwBeamServer();
	}
}

void ARPGSorcererPlayerCharacter::ActivateBlackholwBeamServer_Implementation()
{
	ActivateBlackholwBeamMulticast();
}

void ARPGSorcererPlayerCharacter::ActivateBlackholwBeamMulticast_Implementation()
{
	ActivateBlackholwBeam();
}

void ARPGSorcererPlayerCharacter::ActivateBlackholwBeam()
{
	if (HasAuthority()) return;
	if (BlackholeBeamParticle)
	{
		const USkeletalMeshSocket* StartSocket = GetMesh()->GetSocketByName("FX_Beam");
		const FTransform SocketTransform = StartSocket->GetSocketTransform(GetMesh());
		BlackholeBeamParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BlackholeBeamParticle, SocketTransform);
		if (BlackholeBeamParticleComp)
		{
			FVector Location = TargetingHitResult.ImpactPoint;
			Location.Z += 300.f;
			BlackholeBeamParticleComp->SetVectorParameter(FName("Target"), Location);
		}
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
	if (BlackholeBeamParticleComp)
	{
		BlackholeBeamParticleComp->Deactivate();
	}
}

void ARPGSorcererPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGSorcererPlayerCharacter, SphereTraceLocation);
}

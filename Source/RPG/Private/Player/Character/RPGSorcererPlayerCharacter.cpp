

#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "Player/AnimInstance/RPGSorcererAnimInstance.h"
#include "Player/CharacterAbility/RPGBlackhole.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Projectile/RPGBaseProjectile.h"
#include "Projectile/RPGRestrictionProjectile.h"
#include "GameSystem/ProjectilePoolerComponent.h"
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

	PrimaryProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Primary Projectile Pooler"));

	RestrictionProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Restriction Projectile Pooler"));

	MeteorliteProjPooler = CreateDefaultSubobject<UProjectilePoolerComponent>(TEXT("Meteorlite Projectile Pooler"));
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
	SetSkillPowerCorrectionValues(0.f, 2.f, 1.3f, 0.f);
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
		PrimaryProjPooler->CreatePool(PrimaryPorjectile, 50, EProjectileType::EPT_Sorcerer_Primary);
		RestrictionProjPooler->CreatePool(RestrictionPorjectile, 5, EProjectileType::EPT_Sorcerer_Restriction);
		MeteorliteProjPooler->CreatePool(MeteorlitePorjectile, 5, EProjectileType::EPT_Sorcerer_Meteorlite);
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// R ��ų ������ ��ų�� ��Ʈ�� �ִϸ��̼� ���
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
	Super::CastAbilityAfterTargeting();

	RPGAnimInstance->PlayAbilityMontageOfKey();
	URPGSorcererAnimInstance* SAnimInstance = Cast<URPGSorcererAnimInstance>(RPGAnimInstance);
	if (SAnimInstance) SAnimInstance->AimingPoseOff();
	if (HasAuthority()) UsingMana(RPGAnimInstance->GetCurrentKeyState());
	if (IsLocallyControlled())
	{
		TargetingCompOff();
	}
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
	URPGSorcererAnimInstance* SAnimInstance = Cast<URPGSorcererAnimInstance>(RPGAnimInstance);
	if (SAnimInstance) SAnimInstance->AimingPoseOff();
}

/** --------------------------- �Ϲ� ���� --------------------------- */

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	if (IsLocallyControlled())
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
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_R2")).GetLocation();
	}
	else
	{
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L2")).GetLocation();
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

	SpawnProjectile(EProjectileType::EPT_Sorcerer_Primary, SpawnPoint, SpawnDirection);
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

/** --------------------------- Q ��ų --------------------------- */

void ARPGSorcererPlayerCharacter::FireRestrictionBall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_Q_FireRestrictionBall) return;

	if (IsLocallyControlled())
	{
		FireRestrictionBallServer();
	}
}

void ARPGSorcererPlayerCharacter::FireRestrictionBallServer_Implementation()
{
	SpawnRestrictionProjectile();
}

void ARPGSorcererPlayerCharacter::SpawnRestrictionProjectile()
{
	FVector SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L")).GetLocation();
	FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnPoint).Rotation();

	SpawnProjectile(EProjectileType::EPT_Sorcerer_Restriction, SpawnPoint, SpawnDirection);
}

/** --------------------------- W ��ų --------------------------- */

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

/** --------------------------- E ��ų --------------------------- */

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

/** --------------------------- R ��ų --------------------------- */

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
		SpawnParticle(BlackholeParticle, Location);
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
	DOREPLIFETIME(ARPGSorcererPlayerCharacter, bFloatCharacter);
}



#include "Player/Character/RPGWarriorPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Projectile/RPGBaseProjectile.h"
#include "../RPG.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemlibrary.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

ARPGWarriorPlayerCharacter::ARPGWarriorPlayerCharacter()
{
	
}

void ARPGWarriorPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGWarriorPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGWarriorPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ARPGWarriorPlayerCharacter::OnComponentBeginOverlap);
	}

	if (GetRPGAnimInstance())
	{
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Wield"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("RevealEnemies"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SmashDown"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Rebirth"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MoveToTargettedLocation"));
	}

	if (EnforceParticle)
	{
		EnforceParticleComp = UGameplayStatics::SpawnEmitterAttached(EnforceParticle, GetMesh(), NAME_None, FVector(50.f,0.f,180.f), GetActorRotation(), EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);
	}
}

void ARPGWarriorPlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	if (bReflectOn == false && HasAuthority())
	{
		Super::TakeAnyDamage(DamagedActor, Damage, DamageType, InstigatorController, DamageCauser);
	}
}

void ARPGWarriorPlayerCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bReflectOn && HasAuthority())
	{
		ARPGBaseProjectile* Proj = Cast<ARPGBaseProjectile>(OtherActor);
		if (Proj == nullptr) return;

		Proj->ReflectProjectileFromAllClients();
		PlayReflectMontageAndParticleMulticast(OtherActor->GetActorLocation());
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// W 스킬만 에이밍 X, R 스킬만 인트로 애니메이션 재생
	if (KeyType == EPressedKey::EPK_W)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
	}
	else
	{
		if (KeyType == EPressedKey::EPK_R)
		{
			RPGAnimInstance->AimingPoseOn();
			RPGAnimInstance->PlayAbilityMontageOfKey();
		}
		if (IsLocallyControlled())
		{
			bAiming = true;
			AimCursor->SetVisibility(true);
		}
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

}

void ARPGWarriorPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	if (IsLocallyControlled())
	{
		NormalAttackLineTraceServer();
	}
}

void ARPGWarriorPlayerCharacter::NormalAttackLineTraceServer_Implementation()
{
	NormalAttackLineTrace();
}

void ARPGWarriorPlayerCharacter::NormalAttackLineTrace()
{
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		GetActorLocation() + GetActorForwardVector() * 150,
		GetActorLocation() + GetActorForwardVector() * 150,
		200,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		NormalAttackHitResults,
		true
	);
	for (FHitResult Hit : NormalAttackHitResults)
	{
		UGameplayStatics::ApplyDamage(Hit.GetActor(), 50.f, GetController(), this, UDamageType::StaticClass());
		SpawnNormalAttackImpactParticleMulticast(Hit.GetActor()->GetActorLocation());
	}
}

void ARPGWarriorPlayerCharacter::SpawnNormalAttackImpactParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	SpawnParticle(NormalAttackImpactParticle, SpawnLocation);
}

void ARPGWarriorPlayerCharacter::Wield(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_Q_Wield) return;
	
	if (IsLocallyControlled())
	{
		WieldServer();
	}
}

void ARPGWarriorPlayerCharacter::WieldServer_Implementation()
{
	SphereTrace(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 150.f, 300.f);
	ApplyWieldEffectToHittedActors();
}

void ARPGWarriorPlayerCharacter::SphereTrace(const FVector& Start, const FVector& End, const float& Radius)
{
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		Start,
		End,
		Radius,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		AbilityHitResults,
		true
	);
}

void ARPGWarriorPlayerCharacter::ApplyWieldEffectToHittedActors()
{
	for (FHitResult Hit : AbilityHitResults)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy)
		{
			UGameplayStatics::ApplyDamage(Enemy, 25.f, GetController(), this, UDamageType::StaticClass());
		}
		else
		{
			ARPGBaseProjectile* Proj = Cast<ARPGBaseProjectile>(Hit.GetActor());
			if (Proj)
			{
				Proj->DeactivateProjectileToAllClients();
			}
			SpawnWieldImpactParticleMulticast(Proj->GetActorLocation());
		}
	}
}

void ARPGWarriorPlayerCharacter::SpawnWieldImpactParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	if (HasAuthority() == false)
	{
		SpawnParticle(WieldImpactParticle, SpawnLocation, GetActorRotation());
	}
}

void ARPGWarriorPlayerCharacter::RevealEnemies(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_W_RevealEnemies) return;
	// TODO : 같은 맵에 있는 적들만 감지
	if (IsLocallyControlled())
	{
		RevealEnemiesServer();
	}
}

void ARPGWarriorPlayerCharacter::RevealEnemiesServer_Implementation()
{
	SphereTrace(GetActorLocation(), GetActorLocation(), 1000.f);
	ActivateReflect();
	ActivateEnforceParticleMulticast();
}

void ARPGWarriorPlayerCharacter::ActivateReflect()
{
	CDepthEnemies.Empty();
	for (FHitResult Hit : AbilityHitResults)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy == nullptr || IsValid(Enemy) == false) continue;
		CDepthEnemies.Add(Enemy);
	}
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECR_Overlap);
	bReflectOn = true;
}

void ARPGWarriorPlayerCharacter::OnRep_bReflectOn()
{
	if (bReflectOn)
	{
		EnemyCustomDepthOn();
	}
}

void ARPGWarriorPlayerCharacter::EnemyCustomDepthOn()
{
	for (ARPGBaseEnemyCharacter* Enemy : CDepthEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->OnRenderCustomDepthEffect();
		}
	}
	GetWorldTimerManager().SetTimer(DeactivateRevealEnemiesTimer, this, &ARPGWarriorPlayerCharacter::DeactivateRevealEnemies, 15.f);
}

void ARPGWarriorPlayerCharacter::DeactivateRevealEnemies()
{
	for (ARPGBaseEnemyCharacter* Enemy : CDepthEnemies)
	{
		Enemy->OffRenderCustomDepthEffect();
	}
}

void ARPGWarriorPlayerCharacter::ActivateEnforceParticleMulticast_Implementation()
{
	if (HasAuthority() == false)
	{
		if (EnforceParticleComp) EnforceParticleComp->Activate();
	}
	GetWorldTimerManager().SetTimer(EnforceParticleTimer, this, &ARPGWarriorPlayerCharacter::DeactivateEnforceParticle, 15.f);
}

void ARPGWarriorPlayerCharacter::DeactivateEnforceParticle()
{
	if (HasAuthority() == false)
	{
		EnforceParticleComp->Deactivate();
		SpawnParticle(EnforceEndParticle, GetActorLocation(), GetActorRotation());
	}
	else
	{
		bReflectOn = false;
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECR_Block);
	}
}

void ARPGWarriorPlayerCharacter::PlayReflectMontageAndParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	if (HasAuthority() == false)
	{
		RPGAnimInstance->PlayReflectMontage();
		SpawnParticle(NormalAttackImpactParticle, SpawnLocation, FRotator::ZeroRotator);
	}
}

void ARPGWarriorPlayerCharacter::SmashDown(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_E_SmashDown) return;

	if (IsLocallyControlled())
	{
		SmashDownServer();
	}
}

void ARPGWarriorPlayerCharacter::SmashDownServer_Implementation()
{
	SphereTrace(GetActorLocation(), GetActorLocation(), 600.f);
	SmashDownToEnemies();
}

void ARPGWarriorPlayerCharacter::SmashDownToEnemies()
{
	SmashedEnemies.Empty();
	for (FHitResult Hit : AbilityHitResults)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy == nullptr || IsValid(Enemy) == false) continue;
		SmashedEnemies.Add(Enemy);

		Enemy->LaunchCharacter(Enemy->GetActorForwardVector() * -700.f, false, true);
		Enemy->FalldownToAllClients();
		UGameplayStatics::ApplyDamage(Enemy, 50.f, GetController(), this, UDamageType::StaticClass());

		GetWorldTimerManager().SetTimer(EnemyGetupTimer, this, &ARPGWarriorPlayerCharacter::GetupEnemies, 3.f);
	}
}

void ARPGWarriorPlayerCharacter::GetupEnemies()
{
	for (ARPGBaseEnemyCharacter* Enemy : SmashedEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->GetupToAllClients();
		}
	}
}

void ARPGWarriorPlayerCharacter::Rebirth(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_R_Rebirth) return;
	// TODO : 타게팅 시간 제한 추가
	// TODO : 타게팅 위치로 캐릭터 이동

	if (IsLocallyControlled())
	{
		RebirthServer();
	}
}

void ARPGWarriorPlayerCharacter::RebirthServer_Implementation()
{
	SphereTrace(GetActorLocation(), GetActorLocation(), 1000.f);
	CharacterMoveToTargettedLocationMulticast();
	OneShotKill();
}

void ARPGWarriorPlayerCharacter::MoveToTargettedLocation(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_R_MoveToTargettedLocation) return;

	if (IsLocallyControlled())
	{
		CharacterMoveToTargettedLocationServer();
	}
}

void ARPGWarriorPlayerCharacter::CharacterMoveToTargettedLocationServer_Implementation()
{
	CharacterMoveToTargettedLocationMulticast();
}

void ARPGWarriorPlayerCharacter::CharacterMoveToTargettedLocationMulticast_Implementation()
{
	TargetingHitResult.ImpactPoint.Z = GetActorLocation().Z;
	SetActorLocation(TargetingHitResult.ImpactPoint);
}

void ARPGWarriorPlayerCharacter::OneShotKill()
{
	for (FHitResult Hit : AbilityHitResults)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Hit.GetActor());
		if (Enemy)
		{
			Enemy->InstanceDeath();
		}
	}
}

void ARPGWarriorPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, bReflectOn, COND_AutonomousOnly);
	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, CDepthEnemies, COND_AutonomousOnly);
	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, SmashedEnemies, COND_AutonomousOnly);
}

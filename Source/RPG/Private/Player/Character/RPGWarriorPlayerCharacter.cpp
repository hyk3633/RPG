

#include "Player/Character/RPGWarriorPlayerCharacter.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "Player/AnimInstance/RPGWarriorAnimInstance.h"
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

	SetAbilityCooldownTime(5, 8, 12, 30);
	SetAbilityManaUsage(15, 30, 80, 150);
	SetSkillPowerCorrectionValues(1.5f, 0.f, 3.f, 0.f);
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
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Dash"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SmashDown"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MoveToTargettedLocation"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Rebirth"));
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

	// Q, W ��ų�� ���̹� X
	if (KeyType == EPressedKey::EPK_Q || KeyType == EPressedKey::EPK_W)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
		if (HasAuthority()) UsingMana(RPGAnimInstance->GetCurrentKeyState());
		if (IsLocallyControlled())
		{
			AbilityActiveBitSet(RPGAnimInstance->GetCurrentKeyState());
		}
	}
	else
	{
		if (IsLocallyControlled())
		{
			bAiming = true;
			AimCursor->SetVisibility(true);
			if (KeyType == EPressedKey::EPK_E)
			{
				TargetingCompOn(400);
			}
			else
			{
				TargetingCompOn(600);
			}
		}
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	RPGAnimInstance->PlayAbilityMontageOfKey();
	if (HasAuthority()) UsingMana(RPGAnimInstance->GetCurrentKeyState());
	if (IsLocallyControlled())
	{
		TargetingCompOff();
	}
}

void ARPGWarriorPlayerCharacter::OnAbilityEnded(EPressedKey KeyType)
{
	if (IsLocallyControlled())
	{
		switch (KeyType)
		{
		case EPressedKey::EPK_Q:
			AbilityActiveBitOff(EPressedKey::EPK_Q);
			AbilityCooldownStartServer(EPressedKey::EPK_Q);
			break;
		case EPressedKey::EPK_E:
			AbilityActiveBitOff(EPressedKey::EPK_E);
			AbilityCooldownStartServer(EPressedKey::EPK_E);
			break;
		case EPressedKey::EPK_R:
			AbilityActiveBitOff(EPressedKey::EPK_R);
			AbilityCooldownStartServer(EPressedKey::EPK_R);
			break;
		}
	}
}

/** --------------------------- �Ϲ� ���� --------------------------- */

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
		PLOG(TEXT("%f"), GetStrikingPower());
		UGameplayStatics::ApplyDamage(Hit.GetActor(), GetStrikingPower(), GetController(), this, UDamageType::StaticClass());
		SpawnNormalAttackImpactParticleMulticast(Hit.GetActor()->GetActorLocation());
	}
}

void ARPGWarriorPlayerCharacter::SpawnNormalAttackImpactParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	SpawnParticle(NormalAttackImpactParticle, SpawnLocation);
}

/** --------------------------- Q ��ų --------------------------- */

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
			UGameplayStatics::ApplyDamage(Enemy, GetSkillPower(EPressedKey::EPK_Q), GetController(), this, UDamageType::StaticClass());
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

/** --------------------------- W ��ų --------------------------- */

void ARPGWarriorPlayerCharacter::RevealEnemies(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_W_RevealEnemies) return;
	// TODO : ���� �ʿ� �ִ� ���鸸 ����
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
			Enemy->OnRenderCustomDepthEffect(10);
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
		if(IsLocallyControlled()) AbilityActiveBitOff(EPressedKey::EPK_W);
	}
	else
	{
		bReflectOn = false;
		GetCapsuleComponent()->SetGenerateOverlapEvents(false);
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_EnemyProjectile, ECR_Block);
		AbilityCooldownStart(EPressedKey::EPK_W);
	}
}

/** --------------------------- E ��ų --------------------------- */

void ARPGWarriorPlayerCharacter::Dash(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_E_Dash) return;

	if (IsLocallyControlled())
	{
		DashServer();
	}
}

void ARPGWarriorPlayerCharacter::DashServer_Implementation()
{
	const FVector Direction = (TargetingHitResult.ImpactPoint - GetActorLocation()).GetSafeNormal();
	const float LaunchPower = FVector::Dist2D(TargetingHitResult.ImpactPoint, GetActorLocation());
	DashMulticast(Direction * LaunchPower * 20);
}

void ARPGWarriorPlayerCharacter::DashMulticast_Implementation(const FVector_NetQuantize& DashDirection)
{
	LaunchCharacter(DashDirection, true, true);
}

void ARPGWarriorPlayerCharacter::PlayReflectMontageAndParticleMulticast_Implementation(const FVector_NetQuantize& SpawnLocation)
{
	if (HasAuthority() == false)
	{
		URPGWarriorAnimInstance* WAnimInstance = Cast<URPGWarriorAnimInstance>(RPGAnimInstance);
		if(WAnimInstance) WAnimInstance->PlayReflectMontage();
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
	SphereTrace(GetActorLocation(), GetActorLocation(), 400.f);
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
		UGameplayStatics::ApplyDamage(Enemy, GetSkillPower(EPressedKey::EPK_E), GetController(), this, UDamageType::StaticClass());

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

/** --------------------------- E ��ų --------------------------- */

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

void ARPGWarriorPlayerCharacter::Rebirth(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_R_Rebirth) return;
	// TODO : Ÿ���� �ð� ���� �߰�
	// TODO : Ÿ���� ��ġ�� ĳ���� �̵�

	if (IsLocallyControlled())
	{
		RebirthServer();
	}
}

void ARPGWarriorPlayerCharacter::RebirthServer_Implementation()
{
	SphereTrace(GetActorLocation(), GetActorLocation(), 600.f);
	OneShotKill();
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

	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, bReflectOn, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, CDepthEnemies, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARPGWarriorPlayerCharacter, SmashedEnemies, COND_OwnerOnly);
}


#include "Enemy/Boss/RPGBossEnemyCharacter.h"
#include "Enemy/Boss/RPGBossEnemyAnimInstance.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "../RPG.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

ARPGBossEnemyCharacter::ARPGBossEnemyCharacter()
{
	TargetLocation = FVector::ZeroVector;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetAllowAnimCurveEvaluation(true);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -190));
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	GetCapsuleComponent()->SetCapsuleHalfHeight(200);
	GetCapsuleComponent()->SetCapsuleRadius(170);

	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 220.f));

	AttackRangeMark = CreateDefaultSubobject<UStaticMeshComponent>("Attack Range Mark");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	if (StaticMeshAsset.Succeeded()) { AttackRangeMark->SetStaticMesh(StaticMeshAsset.Object); }
	AttackRangeMark->SetupAttachment(RootComponent);
	AttackRangeMark->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackRangeMark->SetCastShadow(false);
	AttackRangeMark->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UMaterial> CircleMaterialAsset(TEXT(".Material'/Game/_Assets/Materials/BossAttackRange/M_BossAttackRangeCircle.M_BossAttackRangeCircle'"));
	if (CircleMaterialAsset.Succeeded()) { Circle = CircleMaterialAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UMaterial> RectangleMaterialAsset(TEXT("Material'/Game/_Assets/Materials/BossAttackRange/M_BossAttackRangeRectangle.M_BossAttackRangeRectangle'"));
	if (RectangleMaterialAsset.Succeeded()) { Rectangle = RectangleMaterialAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PrimaryAttackParticleAsset(TEXT("ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/PrimaryAttack/FX/P_PrimeHelix_Attack.P_PrimeHelix_Attack'"));
	if (PrimaryAttackParticleAsset.Succeeded()) { PrimaryAttackParticle = PrimaryAttackParticleAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PrimaryAttackWorldImpactParticleAsset(TEXT("ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/PrimaryAttack/FX/P_PrimeHelix_Ground_Impact.P_PrimeHelix_Ground_Impact'"));
	if (PrimaryAttackWorldImpactParticleAsset.Succeeded()) { PrimaryAttackWorldImpactParticle = PrimaryAttackWorldImpactParticleAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PrimaryAttackCharacterImpactParticleAsset(TEXT("ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/PrimaryAttack/FX/Helix_PrimaryImpact.Helix_PrimaryImpact'"));
	if (PrimaryAttackCharacterImpactParticleAsset.Succeeded()) { PrimaryAttackCharacterImpactParticle = PrimaryAttackCharacterImpactParticleAsset.Object; }

	static ConstructorHelpers::FObjectFinder<UParticleSystem> RangeAttackImpactParticleAsset(TEXT("ParticleSystem'/Game/ParagonMinions/FX/Particles/Minions/Prime_Helix/Abilities/SpecialAttack3/FX/P_Prime_SA03_Impact.P_Prime_SA03_Impact'"));
	if (RangeAttackImpactParticleAsset.Succeeded()) { RangeAttackImpactParticle = RangeAttackImpactParticleAsset.Object; }
}

void ARPGBossEnemyCharacter::Tick(float DeltaTime)
{
	if (HasAuthority() == false) return;

	if (bIsAttacking && BossAnimInst)
	{
		CalculateAimYawAndPitch(DeltaTime);
	}

	if (bIsStunned == false)
	{
		StiffTimeLimit += DeltaTime;
		if (CumulativeDamage >= 500 || StiffTimeLimit > 3.f)
		{
			StiffTimeLimit = 0.f;
			CumulativeDamage = 0;
			if (CumulativeDamage >= 500)
			{
				bIsStunned = true;
				if (MyController) MyController->SetIsStunned(true);
				if (BossAnimInst) BossAnimInst->SetStunned(true);
				GetWorldTimerManager().SetTimer(StunTimer, this, &ARPGBossEnemyCharacter::StunCleared, 3.f);
			}
		}
	}
}

void ARPGBossEnemyCharacter::ActivateEnemy(const FVector& Location)
{
	Super::ActivateEnemy(Location);

	GetWorldTimerManager().SetTimer(SpecialAttackCooldownTimer, this, &ARPGBossEnemyCharacter::SpecialAttackCooldownEnd, 10);
}

void ARPGBossEnemyCharacter::CalculateAimYawAndPitch(float DeltaTime)
{
	if (BossAnimInst->GetIsTurning())
	{
		LastframeInitYaw = InitYaw;
		InitYaw = FMath::FInterpConstantTo(InitYaw, TargetAimYaw, DeltaTime, 100.f);
		FRotator R = GetActorRotation();
		R.Yaw += InitYaw - LastframeInitYaw;
		SetActorRotation(R);
		AimYaw = FMath::FInterpConstantTo(AimYaw, 0.f, DeltaTime, 100.f);
	}
	else
	{
		const FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation());

		AimPitch = UKismetMathLibrary::FInterpTo_Constant(AimPitch, DeltaRotation.Pitch, DeltaTime, 200.f);
		BossAnimInst->SetPitch(AimPitch);

		AimYaw = UKismetMathLibrary::FInterpTo_Constant(AimYaw, DeltaRotation.Yaw, DeltaTime, 200.f);
		if (AimYaw >= 60.f || AimYaw <= -60.f)
		{
			SetAimYawMulticast(AimYaw);
			TargetAimYaw = AimYaw > 0 ? 70 : -70;
		}
		InitYaw = 0.f;
	}
	BossAnimInst->SetYaw(AimYaw);
}

void ARPGBossEnemyCharacter::StunCleared()
{
	bIsStunned = false;
	if (MyController) MyController->SetIsStunned(false);
	if (BossAnimInst) BossAnimInst->SetStunned(false);
}

void ARPGBossEnemyCharacter::OnRep_AimPitch()
{
	if (BossAnimInst) BossAnimInst->SetPitch(AimPitch);
}

void ARPGBossEnemyCharacter::OnRep_AimYaw()
{
	if (BossAnimInst) BossAnimInst->SetYaw(AimYaw);
}

void ARPGBossEnemyCharacter::SetAimYawMulticast_Implementation(const float& NewYaw)
{
	if (HasAuthority() == false)
	{
		if (BossAnimInst) BossAnimInst->SetYaw(NewYaw);
	}
}

void ARPGBossEnemyCharacter::HealthDecrease(const int32& Damage)
{
	Super::HealthDecrease(Damage);
	CumulativeDamage += Damage;
}

void ARPGBossEnemyCharacter::InitAnimInstance()
{
	Super::InitAnimInstance();

	BossAnimInst = Cast<URPGBossEnemyAnimInstance>(MyAnimInst);
	if (BossAnimInst)
	{
		BossAnimInst->BindBossEnemyFunction();
		BossAnimInst->DOnSpecialAttack.AddUFunction(this, FName("EmitShockWave"));
		BossAnimInst->DOnSpecialAttack.AddUFunction(this, FName("FireMortar"));
		BossAnimInst->DOnSpecialAttack.AddUFunction(this, FName("Bulldoze"));
		BossAnimInst->DOnSpecialAttackEnd.AddUFunction(this, FName("OnSpecialAttackMontageEnded"));
	}
}

void ARPGBossEnemyCharacter::BTTask_Attack()
{
	bIsAttacking = true;
	if (GetTarget())
	{
		TargetLocation = GetTarget()->GetActorLocation();
	}
	GetWorldTimerManager().SetTimer(AttackDelayTimer, this, &ARPGBossEnemyCharacter::PlayMeleeAttackEffectMulticast, 1.f);
}

void ARPGBossEnemyCharacter::Attack()
{
	if (HasAuthority())
	{
		TArray<FVector> ImpactLocations;
		TArray<FRotator> ImpactRotations;
		EnemyForm->StraightMultiAttack(this, TargetLocation, ImpactLocations, ImpactRotations);
		if (ImpactLocations.Num())
		{
			for (int32 Idx = 0; Idx < ImpactLocations.Num(); Idx++)
			{
				SpawnParticleMulticast(PrimaryAttackCharacterImpactParticle, ImpactLocations[Idx], ImpactRotations[Idx]);
			}
		}
		else
		{
			FHitResult HitResult;
			const FVector TraceStart = GetMesh()->GetSocketTransform(FName("Melee_Socket")).GetLocation();
			const FVector TraceEnd = GetMesh()->GetSocketTransform(FName("Melee_Socket")).GetRotation().Vector() * 1000;
			GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_GroundTrace);
			if (HitResult.bBlockingHit)
			{
				SpawnParticleMulticast(PrimaryAttackWorldImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
		}
	}
}

void ARPGBossEnemyCharacter::SpawnParticleMulticast_Implementation(UParticleSystem* Particle, const FVector_NetQuantize& Location, const FRotator& Rotation)
{
	if (HasAuthority() == false)
	{
		SpawnParticle(Particle, Location, Rotation);
	}
}

void ARPGBossEnemyCharacter::BTTask_SpecialAttack()
{
	MyController->SetCanSpecialAttack(false);
	bIsAttacking = true;

	const ESpecialAttackType SpecialAttackType = StaticCast<ESpecialAttackType>(FMath::RandRange(0, 2));
	
	if (SpecialAttackType == ESpecialAttackType::ESAT_EmitShockWave)
	{
		ActivateAttackRangeMarkMulticast(SpecialAttackType, 500);
		GetWorldTimerManager().SetTimer(SpecialAttackTimer, this, &ARPGBossEnemyCharacter::PlayEmitShockwaveMontageEffectMulticast, 2);
	}
	else if (SpecialAttackType == ESpecialAttackType::ESAT_FireMortar)
	{
		PlayFireMortarMontageEffectMulticast();
	}
	else if (SpecialAttackType == ESpecialAttackType::ESAT_Bulldoze)
	{
		ActivateAttackRangeMarkMulticast(SpecialAttackType, 430);
		GetWorldTimerManager().SetTimer(SpecialAttackTimer, this, &ARPGBossEnemyCharacter::PlayBulldozeMontageEffectMulticast, 2);
	}
}

void ARPGBossEnemyCharacter::PlayEmitShockwaveMontageEffectMulticast_Implementation()
{
	if(BossAnimInst) BossAnimInst->PlayEmitShockWaveMontage();
}

void ARPGBossEnemyCharacter::PlayFireMortarMontageEffectMulticast_Implementation()
{
	if (BossAnimInst) BossAnimInst->PlayFireMortarMontage();
}

void ARPGBossEnemyCharacter::PlayBulldozeMontageEffectMulticast_Implementation()
{
	if (BossAnimInst) BossAnimInst->PlayBulldozeMontage();
}

void ARPGBossEnemyCharacter::ActivateAttackRangeMarkMulticast_Implementation(ESpecialAttackType Type, const float Size)
{
	AttackRangeMark->SetVisibility(true);
	if (Type == ESpecialAttackType::ESAT_EmitShockWave)
	{
		AttackRangeMark->SetRelativeLocation(FVector(0, 0, -200));
		AttackRangeMark->SetWorldScale3D(FVector(Size / 30 - 2, Size / 30 - 2, 1));
		AttackRangeMark->SetMaterial(0, Circle);
	}
	else if (Type == ESpecialAttackType::ESAT_Bulldoze)
	{
		AttackRangeMark->SetRelativeLocation(FVector(450, 0, -200));
		AttackRangeMark->SetWorldScale3D(FVector(Size / 30 - 2, 4, 1));
		AttackRangeMark->SetMaterial(0, Rectangle);
	}
}

void ARPGBossEnemyCharacter::EmitShockWave(ESpecialAttackType Type)
{
	if (HasAuthority() && Type == ESpecialAttackType::ESAT_EmitShockWave)
	{
		TArray<FVector> ImpactLocations;
		TArray<FRotator> ImpactRotations;
		EnemyForm->SphericalRangeAttack(this, 500, ImpactLocations, ImpactRotations);
		if (ImpactLocations.Num())
		{
			for (int32 Idx = 0; Idx < ImpactLocations.Num(); Idx++)
			{
				SpawnParticleMulticast(RangeAttackImpactParticle, ImpactLocations[Idx], ImpactRotations[Idx]);
			}
		}
	}
}

void ARPGBossEnemyCharacter::FireMortar(ESpecialAttackType Type)
{
	if (HasAuthority() && Type == ESpecialAttackType::ESAT_FireMortar)
	{
		TArray<ACharacter*> Targets;
		int8 Count = 0;
		float MinDistance = 3000.f;
		for (ARPGBasePlayerCharacter* Target : TActorRange<ARPGBasePlayerCharacter>(GetWorld()))
		{
			if (GetDistanceTo(Target) <= 1000.f)
			{
				Targets.Add(Target);
				if (++Count == 3) break;
			}
		}

		for (ACharacter* Target : Targets)
		{
			EnemyForm->RangedAttack(this, Target);
		}
	}
}

void ARPGBossEnemyCharacter::Bulldoze(ESpecialAttackType Type)
{
	if (HasAuthority() && Type == ESpecialAttackType::ESAT_Bulldoze)
	{
		TArray<FVector> ImpactLocations;
		TArray<FRotator> ImpactRotations;
		EnemyForm->RectangularRangeAttack(this, 300, ImpactLocations, ImpactRotations);
		if (ImpactLocations.Num())
		{
			for (int32 Idx = 0; Idx < ImpactLocations.Num(); Idx++)
			{
				SpawnParticleMulticast(RangeAttackImpactParticle, ImpactLocations[Idx], ImpactRotations[Idx]);
			}
		}
	}
}

void ARPGBossEnemyCharacter::OnAttackMontageEnded()
{
	Super::OnAttackMontageEnded();
	if(HasAuthority()) bIsAttacking = false;
}

void ARPGBossEnemyCharacter::OnSpecialAttackMontageEnded()
{
	if (HasAuthority())
	{
		bIsAttacking = false;
		DOnSpecialAttackEnd.Broadcast();
		GetWorldTimerManager().SetTimer(SpecialAttackCooldownTimer, this, &ARPGBossEnemyCharacter::SpecialAttackCooldownEnd, 10);
	}
	else
	{
		AttackRangeMark->SetVisibility(false);
	}
}

void ARPGBossEnemyCharacter::SpecialAttackCooldownEnd()
{
	MyController->SetCanSpecialAttack(true);
}

void ARPGBossEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGBossEnemyCharacter, TargetLocation);
	DOREPLIFETIME(ARPGBossEnemyCharacter, AimPitch);
	DOREPLIFETIME(ARPGBossEnemyCharacter, AimYaw);
}

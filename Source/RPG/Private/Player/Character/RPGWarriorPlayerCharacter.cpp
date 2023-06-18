

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

	if (IsLocallyControlled())
	{
		GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
		GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ARPGWarriorPlayerCharacter::OnComponentHitEvent);
	}

	if (GetRPGAnimInstance())
	{
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Wield"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("RevealEnemies"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("SmashDown"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("Rebirth"));
	}

	if (EnforceParticle)
	{
		EnforceParticleComp = UGameplayStatics::SpawnEmitterAttached(EnforceParticle, GetMesh(), NAME_None, FVector(50.f,0.f,180.f), GetActorRotation(), EAttachLocation::KeepRelativeOffset, false, EPSCPoolMethod::None, false);
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// W ��ų�� ���̹� X
	if (KeyType == EPressedKey::EPK_W)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
	}
	else
	{
		bAiming = true;
		if(IsLocallyControlled()) AimCursor->SetVisibility(true);
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	if(HasAuthority() == false) RPGAnimInstance->PlayAbilityMontageOfKey();
}

void ARPGWarriorPlayerCharacter::OnComponentHitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ARPGWarriorPlayerCharacter::Wield(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_Q_Wield) return;
	
	if(IsLocallyControlled()) WieldSphereTraceServer();
}

void ARPGWarriorPlayerCharacter::WieldSphereTraceServer_Implementation()
{
	WieldSphereTrace();
}

void ARPGWarriorPlayerCharacter::WieldSphereTrace()
{
	UKismetSystemLibrary::SphereTraceMulti
	(
		this,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 150.f,
		300.f,
		UEngineTypes::ConvertToTraceType(ECC_PlayerAttack),
		false,
		TArray<AActor*>(),
		EDrawDebugTrace::None,
		WieldHitResults,
		true
	);
	for (FHitResult Hit : WieldHitResults)
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

bool ARPGWarriorPlayerCharacter::IsActorInRange(const AActor* Target)
{
	if (GetDistanceTo(Target) > 500.f) 
		return false;

	const FVector ToTargetVector = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	double Dot = FVector::DotProduct(ToTargetVector, GetActorForwardVector());
	if (Dot < 0.3f) 
		return false;

	return true;
}

void ARPGWarriorPlayerCharacter::RevealEnemies(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_W_RevealEnemies) return;
	// TODO : ���� �ʿ� �ִ� ���鸸 ����
	// TODO : ����ü �ڵ� �ݻ� (�ٸ� �Լ�)
	
	if (IsLocallyControlled())
	{
		EnemyCustomDepthOn();
		ActivateEnforceParticleServer();
	}
}

void ARPGWarriorPlayerCharacter::EnemyCustomDepthOn()
{
	bReflectOn = true;
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 1000.f)
		{
			Enemy->OnRenderCustomDepthEffectToAllClients();
		}
	}
}

void ARPGWarriorPlayerCharacter::ActivateEnforceParticleServer_Implementation()
{
	ActivateEnforceParticleMulticast();
}

void ARPGWarriorPlayerCharacter::ActivateEnforceParticleMulticast_Implementation()
{
	if (HasAuthority()) return;
	if (EnforceParticleComp)
	{
		EnforceParticleComp->Activate();
		GetWorldTimerManager().SetTimer(EnforceParticleTimer, this, &ARPGWarriorPlayerCharacter::DeactivateEnforceParticle, 15.f);
	}
}

void ARPGWarriorPlayerCharacter::DeactivateEnforceParticle()
{
	bReflectOn = false;
	EnforceParticleComp->Deactivate();
	SpawnParticle(EnforceEndParticle, GetActorLocation(), GetActorRotation());
}

void ARPGWarriorPlayerCharacter::SmashDown(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_E_SmashDown) return;
	// TODO : ������ ������
	// TODO : �� �������� �ִϸ��̼�
	// TODO : ������ �ֱ�
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 600.f)
		{
			Enemy->LaunchCharacter(Enemy->GetActorForwardVector() * -700.f, false, true);
		}
	}
}

void ARPGWarriorPlayerCharacter::Rebirth(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_W_R_Rebirth) return;
	
	// TODO : ������ �ֱ�
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 1000.f)
		{
			Enemy->AnnihilatedByPlayer();
		}
	}
}

void ARPGWarriorPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}



#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemlibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/DecalComponent.h"

#include "DrawDebugHelpers.h"

ARPGSorcererPlayerCharacter::ARPGSorcererPlayerCharacter()
{
	AimCursor = CreateDefaultSubobject<UStaticMeshComponent>("Aim Cursor");
	AimCursor->SetupAttachment(RootComponent);
	AimCursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//AimCursor->SetVisibility(false);

	MaxCombo = 3;
}

void ARPGSorcererPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �ٴڿ��� �پ� �ֵ���
	if (bAiming)
	{
		DrawTargetingCursor();
	}
	if (bFloatCharacter)
	{
		FVector Loc = GetActorLocation();
		Loc.Z += 1.6f;
		SetActorLocation(Loc);
	}
	if (bBlackholeOn)
	{
		TArray<AActor*> Enemies;
		UKismetSystemLibrary::SphereOverlapActors(
			this,
			TargetingHitResult.ImpactPoint,
			500.f,
			TArray<TEnumAsByte<EObjectTypeQuery>>(),
			ARPGBaseEnemyCharacter::StaticClass(),
			TArray<AActor*>(),
			Enemies
		);
		FVector d = TargetingHitResult.ImpactPoint;
		d.Z += 300.f;
		for (AActor* Actor : Enemies)
		{
			ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
			Enemy->GetMesh()->AddRadialForce(d, 500.f, -2000, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
}

void ARPGSorcererPlayerCharacter::DrawTargetingCursor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr) return;
	FHitResult TraceHitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
	TraceHitResult.Location.Z += 5.f;
	AimCursor->SetWorldLocation(TraceHitResult.Location);
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
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FireRestrictionBall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorliteFall"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("MeteorShower"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("FloatACharacter"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("BlackholeOn"));
		GetRPGAnimInstance()->DMontageNotify.AddUFunction(this, FName("BlackholeOff"));
	}
	if (MeteorShowerParticleComp)
		MeteorShowerParticleComp->Deactivate();
}

void ARPGSorcererPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// R ��ų ������ ��ų�� ��Ʈ�� �ִϸ��̼� ���
	if (KeyType != EPressedKey::EPK_R)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
	}
	RPGAnimInstance->AimingPoseOn();
	bAiming = true;
}

void ARPGSorcererPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	// Ư�� ���� �����ؾ� �ϴ� ��ų�� Ŀ���� ���� ����Ű�� �� ���̶���Ʈ, �ٸ� �κ��� ������ �ߵ��ȵǰ�, �ٸ� ��Ŭ�� ���
	// ������ �����ؼ� �����ϴ� ��� ���� �� ��� ������ ����
	Cast<APlayerController>(GetController())->GetHitResultUnderCursor(ECC_PlayerAttack, false, TargetingHitResult);
	if (TargetingHitResult.bBlockingHit)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey(true);
		RPGAnimInstance->AimingPoseOff();
	}
	else
	{
		// TODO : �ൿ ���
	}
}

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	SpawnProjectile(PrimaryPorjectile, GetActorLocation(), GetActorRotation());
}

ARPGProjectile* ARPGSorcererPlayerCharacter::SpawnProjectile(TSubclassOf<ARPGProjectile> ProjClass, const FVector& SpawnLoc, const FRotator& SpawnRot)
{
	if (ProjClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		ARPGProjectile* Projectile = GetWorld()->SpawnActor<ARPGProjectile>(ProjClass, SpawnLoc, SpawnRot, SpawnParams);
		Projectile->InitPlayerProjectile();
		return Projectile;
	}
	return nullptr;
}

void ARPGSorcererPlayerCharacter::FireRestrictionBall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_Q_FireRestrictionBall) return;

	const FVector SpawnPoint = GetActorLocation() + (GetActorForwardVector() * 50.f);
	const FRotator SpawnDirection = (TargetingHitResult.GetActor()->GetActorLocation() - SpawnPoint).Rotation();
	ARPGProjectile* TempProj = SpawnProjectile(RestrictionBallPorjectile, SpawnPoint, SpawnDirection);
	if (TempProj)
	{
		const ACharacter* TempCha = Cast<ACharacter>(TargetingHitResult.GetActor());
		if (TempCha) TempProj->SetHomingMode(TempCha);
		else CF();
	}
}

void ARPGSorcererPlayerCharacter::MeteorliteFall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_W_MeteorliteFall) return;

	const FVector SpawnPoint = GetActorLocation() + (GetActorUpVector() * 200.f);
	const FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnPoint).Rotation();
	SpawnProjectile(MeteorlitePorjectile, SpawnPoint, SpawnDirection);
}

void ARPGSorcererPlayerCharacter::MeteorShower(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_E_MeteorShower) return;

	TArray<AActor*> Enemies;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		TargetingHitResult.ImpactPoint,
		300.f,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		ARPGBaseEnemyCharacter::StaticClass(),
		TArray<AActor*>(),
		Enemies
	);
	
	if (MeteorPortalParticle)
	{
		FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
		PSpawnLoc.Z = 500.f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorPortalParticle, PSpawnLoc, FRotator::ZeroRotator);
	}
	GetWorldTimerManager().SetTimer(MeteorShowerTimer, this, &ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle, 1.5f, false);
}

void ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	CF();
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle()
{
	FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
	PSpawnLoc.Z = 500.f;
	MeteorShowerParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator, false, EPSCPoolMethod::None, false);
	MeteorShowerParticleComp->OnParticleCollide.AddDynamic(this, &ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide);
	MeteorShowerParticleComp->Activate();
	//FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
	//PSpawnLoc.Z = 500.f;
	//SpawnParticle(MeteorShowerParticle, PSpawnLoc);
	// TODO : ���׷� Ÿ�̸�
}

void ARPGSorcererPlayerCharacter::FloatACharacter(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_FloatACharacter) return;
	
	bFloatCharacter = true;
	GetWorldTimerManager().SetTimer(FloatingTimer, this, &ARPGSorcererPlayerCharacter::DeactivateFloatingCharacter, 2.f, false);
}

void ARPGSorcererPlayerCharacter::BlackholeOn(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_BlackholeOn) return;

	TArray<AActor*> Enemies;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		TargetingHitResult.ImpactPoint,
		500.f,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		ARPGBaseEnemyCharacter::StaticClass(),
		TArray<AActor*>(),
		Enemies
	);
	for (AActor* Actor : Enemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		Enemy->ActivateRagdollMode();
	}

	SpawnParticle(BlackholeParticle, TargetingHitResult.ImpactPoint);
	bBlackholeOn = true;
}

void ARPGSorcererPlayerCharacter::BlackholeOff(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_R_BlackholeOff) return;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bBlackholeOn = false;
	TArray<AActor*> Enemies;
	UKismetSystemLibrary::SphereOverlapActors(
		this,
		TargetingHitResult.ImpactPoint,
		500.f,
		TArray<TEnumAsByte<EObjectTypeQuery>>(),
		ARPGBaseEnemyCharacter::StaticClass(),
		TArray<AActor*>(),
		Enemies
	);
	for (AActor* Actor : Enemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		Enemy->DeactivateRagdollMode();
	}
}

void ARPGSorcererPlayerCharacter::DeactivateFloatingCharacter()
{
	bFloatCharacter = false;
}
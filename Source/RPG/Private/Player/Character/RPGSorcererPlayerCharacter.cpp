

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
#include "Engine/SkeletalMeshSocket.h"

#include "DrawDebugHelpers.h"

ARPGSorcererPlayerCharacter::ARPGSorcererPlayerCharacter()
{
	AimCursor = CreateDefaultSubobject<UStaticMeshComponent>("Aim Cursor");
	AimCursor->SetupAttachment(RootComponent);
	AimCursor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//AimCursor->SetVisibility(false);

	MaxCombo = 4;
}

void ARPGSorcererPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 바닥에만 붙어 있도록
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
			Actor->SetActorLocation(Actor->GetActorLocation() + (d - Actor->GetActorLocation()).GetSafeNormal() * 5.f);
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

	// R 스킬 제외한 스킬만 인트로 애니메이션 재생
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

	// 특정 적을 선택해야 하는 스킬은 커서로 적을 가리키면 적 하이라이트, 다른 부분을 누르면 발동안되게, 다른 좌클로 취소
	// 범위를 지정해서 시전하는 경우 범위 내 모든 적에게 적용
	if (TargetingHitResult.bBlockingHit)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey(true);
		RPGAnimInstance->AimingPoseOff();
	}
	else
	{
		// TODO : 행동 취소
	}
}

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	FVector SpawnPoint;
	if (GetCurrentCombo() % 2 == 0)
	{
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_R")).GetLocation();
	}
	else
	{
		SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L")).GetLocation();
	}

	FRotator Direction;
	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(TargetingHitResult.GetActor());
	if (Enemy && GetDistanceTo(Enemy) <= AttackRange)
	{
		Direction = (Enemy->GetActorLocation() - SpawnPoint).Rotation();
	}
	else
	{
		Direction = (FVector(TargetingHitResult.ImpactPoint.X, TargetingHitResult.ImpactPoint.Y, SpawnPoint.Z) - SpawnPoint).Rotation();
	}

	SpawnProjectile(PrimaryPorjectile, SpawnPoint, Direction)->SetExpireTime(0.7f);
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

	FVector SpawnPoint = GetMesh()->GetSocketTransform(FName("Muzzle_L")).GetLocation();
	FRotator SpawnDirection = (TargetingHitResult.GetActor()->GetActorLocation() - SpawnPoint).Rotation();
	ARPGProjectile* TempProj = SpawnProjectile(RestrictionBallPorjectile, SpawnPoint, SpawnDirection.Add(60.f, 0.f, 0.f));
	if (TempProj)
	{
		const ACharacter* TempCha = Cast<ACharacter>(TargetingHitResult.GetActor());
		if (TempCha)
		{
			TempProj->SetHomingTarget(TempCha);
			TempProj->SetThrowingMode();
		}
	}
}

void ARPGSorcererPlayerCharacter::MeteorliteFall(ENotifyCode NotifyCode)
{
	if (NotifyCode != ENotifyCode::ENC_S_W_MeteorliteFall) return;
	const FVector SpawnPoint = GetActorLocation() + (GetActorUpVector() * 300.f);
	const FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnPoint).Rotation();
	if (MeteorlitePortalParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorlitePortalParticle, SpawnPoint, SpawnDirection);
	}
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
	GetWorldTimerManager().SetTimer(MeteorShowerTimer, this, &ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle, 0.5f, false);
}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle()
{
	FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
	PSpawnLoc.Z = 500.f;
	MeteorShowerParticleComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator, false, EPSCPoolMethod::None, false);
	MeteorShowerParticleComp->OnParticleCollide.AddDynamic(this, &ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide);
	MeteorShowerParticleComp->Activate();
}

void ARPGSorcererPlayerCharacter::OnMeteorShowerParticleCollide(FName EventName, float EmitterTime, int32 ParticleTime, FVector Location, FVector Velocity, FVector Direction, FVector Normal, FName BoneName, UPhysicalMaterial* PhysMat)
{
	CF();
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
		Enemy->EnableSuckedIn();
	}

	FVector d = TargetingHitResult.ImpactPoint;
	d.Z += 300.f;
	SpawnParticle(BlackholeParticle, d);
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
		Actor->Destroy();
	}
}

void ARPGSorcererPlayerCharacter::DeactivateFloatingCharacter()
{
	bFloatCharacter = false;
}

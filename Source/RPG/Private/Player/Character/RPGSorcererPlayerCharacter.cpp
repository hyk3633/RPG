

#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemlibrary.h"
#include "../RPG.h"
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
		GetRPGAnimInstance()->DOnAbility_Q_Cast.BindUFunction(this, FName("FireRestrictionBall"));
		GetRPGAnimInstance()->DOnAbility_W_Cast.BindUFunction(this, FName("MeteorFall"));
		GetRPGAnimInstance()->DOnAbility_E_Cast.BindUFunction(this, FName("MeteorShower"));
		GetRPGAnimInstance()->DOnAbility_R_Cast.BindUFunction(this, FName("SummongBlackhole"));
	}
}

void ARPGSorcererPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// R ��ų ������ ��ų�� ��Ʈ�� �ִϸ��̼� �� ���̹� ���� ���
	if (KeyType != EPressedKey::EPK_R)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
		RPGAnimInstance->AimingPoseOn();
	}
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

void ARPGSorcererPlayerCharacter::FireRestrictionBall()
{
	if (TargetingHitResult.bBlockingHit == false) return;
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

void ARPGSorcererPlayerCharacter::MeteorFall()
{
	if (TargetingHitResult.bBlockingHit == false) return;
	const FVector SpawnPoint = GetActorLocation() + (GetActorUpVector() * 200.f);
	const FRotator SpawnDirection = (TargetingHitResult.ImpactPoint - SpawnPoint).Rotation();
	SpawnProjectile(MeteorPorjectile, SpawnPoint, SpawnDirection);
}

void ARPGSorcererPlayerCharacter::MeteorShower()
{
	if (TargetingHitResult.bBlockingHit == false) return;
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

void ARPGSorcererPlayerCharacter::SummongBlackhole()
{

}

void ARPGSorcererPlayerCharacter::SpawnMeteorShowerParticle()
{
	if (MeteorShowerParticle)
	{
		FVector PSpawnLoc = TargetingHitResult.ImpactPoint;
		PSpawnLoc.Z = 500.f;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MeteorShowerParticle, PSpawnLoc, FRotator::ZeroRotator);
	}
	// TODO : ���׷� Ÿ�̸�
}

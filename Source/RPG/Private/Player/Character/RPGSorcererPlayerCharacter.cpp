

#include "Player/Character/RPGSorcererPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "GameFramework/PlayerController.h"
#include "../RPG.h"
#include "Components/DecalComponent.h"

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

	RPGAnimInstance->PlayAbilityMontageOfKey(true);
	RPGAnimInstance->AimingPoseOff();
}

void ARPGSorcererPlayerCharacter::CastNormalAttack()
{
	Super::CastNormalAttack();

	SpawnProjectile();
}

void ARPGSorcererPlayerCharacter::SpawnProjectile()
{
	if (ProjetileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		ARPGProjectile* Projectile = GetWorld()->SpawnActor<ARPGProjectile>(ProjetileClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		Projectile->InitPlayerProjectile();
	}
}

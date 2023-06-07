

#include "Player/Character/RPGWarriorPlayerCharacter.h"
#include "Player/RPGAnimInstance.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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

	if (GetRPGAnimInstance())
	{
		//GetRPGAnimInstance()->DOnAbility_Q_Cast.BindUFunction(this, FName("FindEnemiesInFrontAndDamage"));
		//GetRPGAnimInstance()->DOnAbility_W_Cast.BindUFunction(this, FName("RevealNearbyEnemies"));
		//GetRPGAnimInstance()->DOnAbility_E_Cast.BindUFunction(this, FName("PushawayNearbyEnemies"));
		//GetRPGAnimInstance()->DOnAbility_R_Cast.BindUFunction(this, FName("AnnihilateNearbyEnemies"));
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityByKey(EPressedKey KeyType)
{
	Super::CastAbilityByKey(KeyType);

	// W 스킬만 에이밍
	if (KeyType == EPressedKey::EPK_W)
	{
		RPGAnimInstance->PlayAbilityMontageOfKey();
	}
	else
	{
		bAiming = true;
	}
}

void ARPGWarriorPlayerCharacter::CastAbilityAfterTargeting()
{
	Super::CastAbilityAfterTargeting();

	RPGAnimInstance->PlayAbilityMontageOfKey();
}

void ARPGWarriorPlayerCharacter::FindEnemiesInFrontAndDamage()
{
	// TODO : 투사체 제거
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (IsActorInRange(Enemy))
		{
			UGameplayStatics::ApplyDamage(Enemy, 25.f, GetController(), this, UDamageType::StaticClass());
		}
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

void ARPGWarriorPlayerCharacter::RevealNearbyEnemies()
{
	// TODO : 같은 맵에 있는 적들만 감지
	// TODO : 투사체 자동 반사 (다른 함수)
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 1000.f)
		{
			Enemy->OnRenderCustomDepthEffect();
		}
	}
}

void ARPGWarriorPlayerCharacter::PushawayNearbyEnemies()
{
	// TODO : 적들이 물러남
	// TODO : 적 쓰러지는 애니메이션
	// TODO : 데미지 주기
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 600.f)
		{
			Enemy->LaunchCharacter(Enemy->GetActorForwardVector() * -700.f, false, true);
		}
	}
}

void ARPGWarriorPlayerCharacter::AnnihilateNearbyEnemies()
{
	// TODO : 데미지 주기
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 1000.f)
		{
			Enemy->AnnihilatedByPlayer();
		}
	}
}

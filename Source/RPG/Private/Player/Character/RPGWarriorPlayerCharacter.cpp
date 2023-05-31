

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
		GetRPGAnimInstance()->DOnAbility_Q_Cast.BindUFunction(this, FName("FindEnemiesInFront"));
		GetRPGAnimInstance()->DOnAbility_W_Cast.BindUFunction(this, FName("RevealNearbyEnemies"));
	}
}

void ARPGWarriorPlayerCharacter::Ability_Q()
{
	Super::Ability_Q();
	
}

void ARPGWarriorPlayerCharacter::Ability_W()
{
	Super::Ability_W();
	
}

void ARPGWarriorPlayerCharacter::Ability_E()
{
	Super::Ability_E();
	
}

void ARPGWarriorPlayerCharacter::Ability_R()
{
	Super::Ability_R();
	
}

void ARPGWarriorPlayerCharacter::FindEnemiesInFront()
{
	// TODO : ����ü ����
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
	// TODO : ���� �ʿ� �ִ� ���鸸 ����
	// TODO : ����ü �ڵ� �ݻ� (�ٸ� �Լ�)
	for (ARPGBaseEnemyCharacter* Enemy : TActorRange<ARPGBaseEnemyCharacter>(GetWorld()))
	{
		if (GetDistanceTo(Enemy) < 1000.f)
		{
			Enemy->OnRenderCustomDepthEffect();
		}
	}
}

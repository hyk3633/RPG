
#include "Player/CharacterAbility/RPGBlackhole.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPG.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ARPGBlackhole::ARPGBlackhole()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	SetRootComponent(CollisionComponent);
	CollisionComponent->SetCollisionObjectType(ECC_PlayerAttack);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_EnemyBody, ECollisionResponse::ECR_Overlap);
	CollisionComponent->SetSphereRadius(500.f);
	CollisionComponent->SetGenerateOverlapEvents(true);
}

void ARPGBlackhole::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBlackhole::ExpireBlackhole, ExpireTime);
	GetWorldTimerManager().SetTimer(DamageTimer, this, &ARPGBlackhole::ApplyDamageToEnemies, 0.3f, true);
}

void ARPGBlackhole::InitiateSuckingUpEnemies()
{
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);

	for (AActor* Actor : OverlappingEnemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		if (Enemy == nullptr || Enemy->GetIsActivated() == false) continue;
		Enemy->EnableSuckedInToAllClients();
	}
}

void ARPGBlackhole::ApplyDamageToEnemies()
{
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);
	for (AActor* Actor : OverlappingEnemies)
	{
		UGameplayStatics::ApplyDamage(Actor, Damage, nullptr, this, UDamageType::StaticClass());
	}
}

void ARPGBlackhole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bBlackholeOn) SuckingUpEnemies();
}

void ARPGBlackhole::SuckingUpEnemies()
{
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);
	for (AActor* Actor : OverlappingEnemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		if (Enemy == nullptr || Enemy->GetIsActivated() == false) continue;
		if(Enemy->GetSuckedIn() == false) Enemy->EnableSuckedInToAllClients();
		Enemy->SetActorLocation(Enemy->GetActorLocation() + (GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal() * 5.f);
	}
}

void ARPGBlackhole::ExpireBlackhole()
{
	bBlackholeOn = false;
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);
	for (AActor* Actor : OverlappingEnemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		if (Enemy == nullptr || Enemy->GetIsActivated() == false) continue;
		Enemy->DisableSuckedInToAllClients();
	}
	SetLifeSpan(0.5f);
}



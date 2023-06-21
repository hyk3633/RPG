
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

}

void ARPGBlackhole::BeginPlay()
{
	Super::BeginPlay();
	

	InitiateSuckingUpEnemies();

	GetWorldTimerManager().SetTimer(ExpireTimer, this, &ARPGBlackhole::ExpireBlackhole, ExpireTime);
}

void ARPGBlackhole::InitiateSuckingUpEnemies()
{
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);
	for (AActor* Actor : OverlappingEnemies)
	{
		ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(Actor);
		if (Enemy == nullptr) continue;
		Enemy->EnableSuckedInToAllClients();
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
		if (Cast<ARPGBaseEnemyCharacter>(Actor) == nullptr) continue;
		Actor->SetActorLocation(Actor->GetActorLocation() + (GetActorLocation() - Actor->GetActorLocation()).GetSafeNormal() * 5.f);
	}
}

void ARPGBlackhole::ExpireBlackhole()
{
	bBlackholeOn = false;
	CollisionComponent->GetOverlappingActors(OverlappingEnemies);
	for (AActor* Actor : OverlappingEnemies)
	{
		if (Cast<ARPGBaseEnemyCharacter>(Actor) == nullptr) continue;
		Actor->Destroy();
	}
	SetLifeSpan(0.5f);
}



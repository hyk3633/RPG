

#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "../RPG.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));
	WorldGridManager->InitWorldGrid(80, 25);

	ItemSpawnManager = CreateDefaultSubobject<UItemSpawnManagerComponent>(TEXT("Item Spawn Manager"));
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY)
{
	WorldGridManager->AStar(Start, Dest, PathToDestX, PathToDestY);
}

void ARPGGameModeBase::SpawnItems(const FVector& Location)
{
	FHitResult Hit;
	FVector TraceEnd = Location;
	TraceEnd.Z -= 200.f;
	GetWorld()->LineTraceSingleByChannel(Hit, Location, TraceEnd, ECC_GroundTrace);

	if (Hit.bBlockingHit)
	{
		ItemSpawnManager->SpawnItems(Hit.ImpactPoint);
	}
	else
	{
		ItemSpawnManager->SpawnItems(Location);
	}
}




#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	WorldGridManagerComponent = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager Component"));
	WorldGridManagerComponent->InitWorldGrid(80, 25);
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<float>& PathToDestX, TArray<float>& PathToDestY)
{
	WorldGridManagerComponent->AStar(Start, Dest, PathToDestX, PathToDestY);
}

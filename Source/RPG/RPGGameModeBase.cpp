

#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "../RPG.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));
	WorldGridManager->InitWorldGrid(80, 25);

	ItemSpawnManager = CreateDefaultSubobject<UItemSpawnManagerComponent>(TEXT("Item Spawn Manager"));

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyDataTable(TEXT("/Game/_Assets/DataTable/DataTable_EnemyData.DataTable_EnemyData"));
	if (Obj_EnemyDataTable.Succeeded()) ItemDataTable = Obj_EnemyDataTable.Object;

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
	ItemSpawnManager->SpawnItems(Location);
}

void ARPGGameModeBase::DropItem(const FItemInfo& Info, const FVector& Location)
{
	ItemSpawnManager->DropItem(Info, Location);
}

FEnemyData* ARPGGameModeBase::GetEnemyData(const EEnemyType Type)
{
	if (ItemDataTable)
	{
		FEnemyData* Data = ItemDataTable->FindRow<FEnemyData>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}


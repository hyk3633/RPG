
#include "GameSystem/EnemyPooler.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "Structs/EnemyInfo.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"

AEnemyPooler::AEnemyPooler()
{
	PrimaryActorTick.bCanEverTick = false;

	EnemyFormComponent = CreateDefaultSubobject<URPGEnemyFormComponent>(TEXT("Enemy Form Component"));
}

void AEnemyPooler::CreatePool(const int32 Size, const EEnemyType Type, const FVector& NewWaitingLocation)
{
	PoolSize = Size;
	EnemyArr.Init(nullptr, Size);
	WaitingLocation = NewWaitingLocation;
	EnemyFormComponent->InitEnemyFormComponent(Type);

	for (uint32 Idx = 0; Idx < PoolSize; Idx++)
	{
		EnemyArr[Idx] = EnemyFormComponent->CreateNewEnemy(WaitingLocation);
	}
}

void AEnemyPooler::DestroyPool()
{
	for (ARPGBaseEnemyCharacter* Enemy : EnemyArr)
	{
		Enemy->Destroy();
	}
	EnemyArr.Empty();
}

ARPGBaseEnemyCharacter* AEnemyPooler::GetPooledEnemy()
{
	for (uint32 Idx = 0; Idx < PoolSize; Idx++)
	{
		if(EnemyArr[Idx]->GetIsActivated() == false)
			return EnemyArr[Idx];
	}

	auto NewEnemy = EnemyFormComponent->CreateNewEnemy(WaitingLocation);
	EnemyArr.Add(NewEnemy);
	PoolSize++;
	return NewEnemy;
}


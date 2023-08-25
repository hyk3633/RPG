
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

void AEnemyPooler::CreatePool(const int32 Size, const EEnemyType Type)
{
	if (HasAuthority() == false) return;

	PoolSize = ActivatedNum = DeactivatedNum = Size;
	EnemyArr.Init(nullptr, Size);

	EnemyFormComponent->InitEnemyFormComponent(Type);

	for (int8 Idx = 0; Idx < PoolSize; Idx++)
	{
		EnemyArr[Idx] = EnemyFormComponent->CreateNewEnemy();
		EnemyArr[Idx]->DOnDeath.AddUFunction(this, FName("AddDeactivatedNum"));
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
	if (DeactivatedNum == 0) return nullptr;
	if (ActivatedNum == 0) ActivatedNum = PoolSize;

	DeactivatedNum--;
	ActivatedNum--;

	return EnemyArr[ActivatedNum];
}

void AEnemyPooler::AddDeactivatedNum()
{
	DeactivatedNum++;
}



#include "AI/BTTask_RangedAttack.h"
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "../RPG.h"

UBTTask_RangedAttack::UBTTask_RangedAttack()
{
	bNotifyTick = true;

	NodeName = TEXT("Ranegd Attack");

	bIsAttacking = true;
	bIsAborted = false;
}

EBTNodeResult::Type UBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARPGRangedEnemyCharacter* OwnerEnemy = Cast<ARPGRangedEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerEnemy)
	{
		OwnerEnemy->BTTask_RangedAttack();
		OwnerEnemy->DOnAttackEnd.AddLambda([this]() -> void { bIsAttacking = false; });
		OwnerEnemy->DOnDeath.AddLambda([this]() -> void { bIsAborted = true; });

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_RangedAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		bIsAttacking = true;
	}
	if (bIsAborted)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		bIsAborted = false;
	}
}

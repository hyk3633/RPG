

#include "AI/BTTask_Attack.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"
#include "../RPG.h"
UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;

	NodeName = TEXT("Attack");

	bIsAttacking = true;
	bIsAborted = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARPGBaseEnemyCharacter* OwnerEnemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerEnemy)
	{
		OwnerEnemy->BTTask_Attack();
		OwnerEnemy->DOnAttackEnd.AddLambda([this]() -> void { bIsAttacking = false; });
		OwnerEnemy->DOnDeath.AddLambda([this]() -> void { bIsAborted = true; });

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

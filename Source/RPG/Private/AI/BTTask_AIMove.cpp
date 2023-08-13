
#include "AI/BTTask_AIMove.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

UBTTask_AIMove::UBTTask_AIMove()
{
	bNotifyTick = true;

	NodeName = TEXT("AI Move To Target");

	bIsMoving = true;
}

EBTNodeResult::Type UBTTask_AIMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARPGBaseEnemyCharacter* OwnerEnemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerEnemy)
	{
		OwnerEnemy->BTTask_Move();
		OwnerEnemy->DMoveEnd.AddLambda([this]() -> void { bIsMoving = false; });

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_AIMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (!bIsMoving)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		bIsMoving = true;
	}
}

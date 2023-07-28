
#include "AI/BTTask_SpecialAttack.h"
#include "Enemy/Boss/RPGBossEnemyCharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "AIController.h"

UBTTask_SpecialAttack::UBTTask_SpecialAttack()
{
	bNotifyTick = true;

	NodeName = TEXT("Attack");

	bIsAttacking = true;
	bIsAborted = false;
}

EBTNodeResult::Type UBTTask_SpecialAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ARPGBossEnemyCharacter* OwnerEnemy = Cast<ARPGBossEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (OwnerEnemy)
	{
		OwnerEnemy->BTTask_SpecialAttack();
		OwnerEnemy->DOnSpecialAttackEnd.AddLambda([this]() -> void { bIsAttacking = false; });
		OwnerEnemy->DOnDeath.AddLambda([this]() -> void { bIsAborted = true; });

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SpecialAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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

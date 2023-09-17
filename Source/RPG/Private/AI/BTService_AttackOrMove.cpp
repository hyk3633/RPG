
#include "AI/BTService_AttackOrMove.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "GameSystem/EnemySpawner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../RPG.h"

UBTService_AttackOrMove::UBTService_AttackOrMove()
{
	NodeName = TEXT("Attack Or Move");
	Interval = 0.1f;
	bNotifyOnSearch = true;
}

void UBTService_AttackOrMove::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr) return;

	if (Enemy->GetIsAttacking()) return;

	ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ARPGEnemyAIController::TargetPlayer));
	if (Player == nullptr) return;

	if (Enemy->GetDistanceTo(Player) <= Enemy->GetAttackDistance())
	{
		if (Enemy->GetAttackType() == EEnemyAttackType::EEAT_Ranged)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanRangedAttack, true);
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanMeleeAttack, true);
		}
	}
	else
	{
		if (Enemy->GetAttackType() == EEnemyAttackType::EEAT_Ranged)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanRangedAttack, false);
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanMeleeAttack, false);
		}
	}
}

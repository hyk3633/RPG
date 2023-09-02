
#include "AI/BTService_RangedOrMelee.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "GameSystem/EnemySpawner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../RPG.h"

UBTService_RangedOrMelee::UBTService_RangedOrMelee()
{
	NodeName = TEXT("Ranged Or Melee");
	Interval = 0.1f;
	bNotifyOnSearch = true;
}

void UBTService_RangedOrMelee::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARPGRangedEnemyCharacter* RangedEnemy = Cast<ARPGRangedEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (RangedEnemy == nullptr) return;

	ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ARPGEnemyAIController::TargetPlayer));
	if (Player == nullptr) return;

	if (RangedEnemy->CheckCanFireToTarget())
	{
		const float DistToTarget = RangedEnemy->GetDistanceTo(Player);
		if (DistToTarget > RangedEnemy->GetAttackDistance() && DistToTarget <= RangedEnemy->GetDetectDistance())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanRangedAttack, true);
			return;
		}
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(ARPGEnemyAIController::CanRangedAttack, false);
}

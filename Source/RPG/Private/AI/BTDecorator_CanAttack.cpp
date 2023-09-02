

#include "AI/BTDecorator_CanAttack.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Character/RPGRangedEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CanAttack::UBTDecorator_CanAttack()
{
	NodeName = TEXT("Can Attack");
}

bool UBTDecorator_CanAttack::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr) return false;

	ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ARPGEnemyAIController::TargetPlayer));
	if (Player == nullptr) return false;

	if (Enemy->GetAttackType() == EEnemyAttackType::EEAT_Melee)
	{
		if (Enemy->GetDistanceTo(Player) <= Enemy->GetAttackDistance()) return true;
	}
	else
	{
		ARPGRangedEnemyCharacter* RangedEnemy = Cast<ARPGRangedEnemyCharacter>(Enemy);
		if (RangedEnemy->CheckCanFireToTarget())
		{
			if (Enemy->GetAttackType() == EEnemyAttackType::EEAT_Hybrid)
			{
				if (Enemy->GetDistanceTo(Player) <= Enemy->GetDetectDistance()) return true;
			}
			else
			{
				if (Enemy->GetDistanceTo(Player) <= Enemy->GetAttackDistance()) return true;
			}
		}
	}

	return false;
}

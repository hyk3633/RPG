

#include "AI/BTDecorator_CanAttack.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
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

	if (Enemy->GetDistanceTo(Player) <= Enemy->GetAttackDistance()) return true;

	return false;
}



#include "AI/BTDecorator_IsAbleToAction.h"
#include "Enemy/RPGEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_IsAbleToAction::UBTDecorator_IsAbleToAction()
{
	NodeName = TEXT("Is able to action");
}

bool UBTDecorator_IsAbleToAction::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ARPGEnemyAIController::IsDead)) return false;
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ARPGEnemyAIController::IsFalldown)) return false;
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ARPGEnemyAIController::IsRestrained)) return false;
	if (OwnerComp.GetBlackboardComponent()->GetValueAsBool(ARPGEnemyAIController::IsSuckedIn)) return false;

	return true;
}

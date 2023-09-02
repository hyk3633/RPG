
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTask_RangedAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	bool bIsAttacking;

	bool bIsAborted;

};

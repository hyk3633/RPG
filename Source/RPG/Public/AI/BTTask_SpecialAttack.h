
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpecialAttack.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTTask_SpecialAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	
	UBTTask_SpecialAttack();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	bool bIsAttacking;

	bool bIsAborted;
};

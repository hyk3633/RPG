
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AIMove.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTTask_AIMove : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTask_AIMove();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:

	bool bIsMoving;

};

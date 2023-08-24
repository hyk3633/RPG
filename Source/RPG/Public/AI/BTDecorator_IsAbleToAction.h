
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsAbleToAction.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTDecorator_IsAbleToAction : public UBTDecorator
{
	GENERATED_BODY()

public:

	UBTDecorator_IsAbleToAction();

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};

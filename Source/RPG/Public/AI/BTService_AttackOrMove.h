
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AttackOrMove.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTService_AttackOrMove : public UBTService
{
	GENERATED_BODY()

public:

	UBTService_AttackOrMove();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};

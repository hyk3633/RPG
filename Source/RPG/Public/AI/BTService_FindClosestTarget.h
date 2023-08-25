
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindClosestTarget.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTService_FindClosestTarget : public UBTService
{
	GENERATED_BODY()

public:

	UBTService_FindClosestTarget();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};

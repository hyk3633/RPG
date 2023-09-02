
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RangedOrMelee.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UBTService_RangedOrMelee : public UBTService
{
	GENERATED_BODY()

public:

	UBTService_RangedOrMelee();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};

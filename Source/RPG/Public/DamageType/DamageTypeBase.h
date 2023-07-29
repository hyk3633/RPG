
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageTypeBase.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UDamageTypeBase : public UDamageType
{
	GENERATED_BODY()

public:

	int32 CalculateDamage(const float& Damage, const float& DefensivePower);
	
};


#pragma once

#include "CoreMinimal.h"
#include "DamageType/DamageTypeBase.h"
#include "DamageTypeStunAndPush.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UDamageTypeStunAndPush : public UDamageTypeBase
{
	GENERATED_BODY()
	
public:

	void GetPushed(AActor* Pusher, ACharacter* Pushed);

};

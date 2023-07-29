
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageTypeStunAndPush.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API UDamageTypeStunAndPush : public UDamageType
{
	GENERATED_BODY()
	
public:

	void GetPushed(AActor* Pusher, ACharacter* Pushed);

};

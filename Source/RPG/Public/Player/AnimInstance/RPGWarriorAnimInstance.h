
#pragma once

#include "CoreMinimal.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "RPGWarriorAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGWarriorAnimInstance : public URPGAnimInstance
{
	GENERATED_BODY()
	
public:

	void PlayReflectMontage();

private:

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* ReflectMontage;

};

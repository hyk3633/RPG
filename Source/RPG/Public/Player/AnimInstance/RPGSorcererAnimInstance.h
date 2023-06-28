
#pragma once

#include "CoreMinimal.h"
#include "Player/AnimInstance/RPGAnimInstance.h"
#include "RPGSorcererAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGSorcererAnimInstance : public URPGAnimInstance
{
	GENERATED_BODY()
	
public:

	FORCEINLINE void AimingPoseOn() { bAimingPose = true; };
	FORCEINLINE void AimingPoseOff() { bAimingPose = false; };

	void PlayAbilityIntroMontage();

private:

	UPROPERTY(BlueprintReadOnly, Category = "Aiming", meta = (AllowPrivateAccess = "true"))
	bool bAimingPose = false;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_Q_IntroMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_W_IntroMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_E_IntroMontage;

};

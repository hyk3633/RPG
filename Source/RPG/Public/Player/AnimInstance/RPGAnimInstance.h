
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/PressedKey.h"
#include "Enums/NotifyCode.h"
#include "RPGAnimInstance.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAttackInputCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackEndedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnDeathEndedDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FMontageNotifyDelegate, ENotifyCode NotifyCode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAbilityMontageEndedDelegate, EPressedKey KeyType);

UCLASS()
class RPG_API URPGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	virtual void NativeInitializeAnimation() override;
	
public:

	void PlayNormalAttackMontage();

	void JumpToAttackMontageSection(int32 NewSection);

	void PlayAbilityMontageOfKey();

	bool IsAbilityERMontagePlaying();

	bool IsNormalAttackMontagePlaying();

	void PlayDeathMontage();

	FORCEINLINE void SetCurrentKeyState(EPressedKey KeyType) { CurrentKeyState = KeyType; }
	FORCEINLINE EPressedKey GetCurrentKeyState() const { return CurrentKeyState; }
	FORCEINLINE void SetMaxCombo(const int8 MaxValue) { MaxCombo = MaxValue; }
	FORCEINLINE void SetJogSpeed(const float NewSpeed) { JogSpeed = NewSpeed; }
	FORCEINLINE void SetNormalAttackSpeed(const float NewSpeed) { NormalAttackSpeed = NewSpeed; }

	FOnAttackInputCheckDelegate DOnAttackInputCheck;

	FOnAttackEndedDelegate DOnAttackEnded;

	FOnDeathEndedDelegate DOnDeathEnded;

	FMontageNotifyDelegate DMontageNotify;

	FOnAbilityMontageEndedDelegate DOnAbilityMontageEnded;

protected:

	void PlayAbilityMontage(UAnimMontage* AbilityMontage);

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAbilityMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable)
	void AnimNotify_AttackInputCheck();

	UFUNCTION(BlueprintCallable)
	void AnimNotify_MontageNotify(ENotifyCode NotifyCode);

private:

	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* NormalAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_Q_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_W_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_E_Montage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	UAnimMontage* Ability_R_Montage;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPressedKey CurrentKeyState = EPressedKey::EPK_None;

	int8 MaxCombo;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float JogSpeed = 1.f;

	float NormalAttackSpeed = 1.f;
};

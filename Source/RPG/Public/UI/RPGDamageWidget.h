
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGDamageWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class RPG_API URPGDamageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void InitDamageWidget();

	void SetDamageTextAndPopup(const int32 Damage);

	FORCEINLINE bool IsWidgetUsable() const { return !bActivated; }

protected:

	UFUNCTION(BlueprintCallable)
	void DeactivateWidget();

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DamageText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* PopUp;

	bool bActivated = false;

};


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGGameplayInterface.generated.h"

/**
 * 
 */

class ARPGHUD;
class UProgressBar;
class UImage;
class URPGInventoryWidget;

UCLASS()
class RPG_API URPGGameplayInterface : public UUserWidget
{
	GENERATED_BODY()
	
private:

	friend ARPGHUD;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockProgress_Q;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockProgress_W;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockProgress_E;

	UPROPERTY(meta = (BindWidget))
	UImage* ClockProgress_R;

	UPROPERTY(meta = (BindWidget))
	URPGInventoryWidget* InventoryWidget;
};

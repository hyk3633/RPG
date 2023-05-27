
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGGameplayInterface.generated.h"

/**
 * 
 */

class ARPGHUD;
class UProgressBar;

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
};

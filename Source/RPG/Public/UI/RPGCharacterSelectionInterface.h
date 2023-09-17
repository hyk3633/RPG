
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGCharacterSelectionInterface.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class RPG_API URPGCharacterSelectionInterface : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Warrior;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Sorcerer;
};

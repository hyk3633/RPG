
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Enums/CharacterType.h"
#include "RPGCharacterSelectionHUD.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterSelectedDelegate, ECharacterType CharacterType);

class URPGCharacterSelectionInterface;

UCLASS()
class RPG_API ARPGCharacterSelectionHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	ARPGCharacterSelectionHUD();

	FOnCharacterSelectedDelegate DOnCharacterSelected;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWarriorSelected();

	UFUNCTION()
	void OnSorcererSelected();

private:

	UPROPERTY()
	TSubclassOf<URPGCharacterSelectionInterface> CharacterSelectWBPClass;

	URPGCharacterSelectionInterface* CharacterSelectionInterface;
};

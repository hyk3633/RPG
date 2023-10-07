
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enums/CharacterType.h"
#include "RPGMainScreenGameMode.generated.h"

/**
 * 
 */

class URPGCharacterSelectionInterface;

UCLASS()
class RPG_API ARPGMainScreenGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	ARPGMainScreenGameMode();

	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void ConnectToMainMap();

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	TSubclassOf<URPGCharacterSelectionInterface> CharacterSelectWBPClass;

	URPGCharacterSelectionInterface* CharacterSelectionInterface;
	
};

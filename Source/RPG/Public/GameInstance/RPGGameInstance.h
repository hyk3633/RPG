
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Enums/CharacterType.h"
#include "RPGGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API URPGGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	URPGGameInstance();

protected:

	virtual void Init() override;

};

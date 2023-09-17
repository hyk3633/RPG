
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

	void SaveUniqueID(const FString& ID);

	void SaveCharacterTypeToSpawn(ECharacterType Type);

	void SaveCharacterTypeToSpawn(const FString& ID, ECharacterType Type);

	ECharacterType GetPlayersCharacterType(const FString& ID);

	FORCEINLINE const FString& GetUniqueID() { return UniqueID; }
	FORCEINLINE ECharacterType GetCharacterType() const { return CharacterType; }

	void Host();

	void Join();

private:

	ECharacterType CharacterType;

	FString UniqueID;

	TMap<FString, ECharacterType> PlayersCharacterTypeMap;
};

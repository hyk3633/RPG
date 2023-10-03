
#include "GameInstance/RPGGameInstance.h"
#include "../RPG.h"

URPGGameInstance::URPGGameInstance()
{

}

void URPGGameInstance::SaveUniqueID(const FString& ID)
{
	UniqueID = ID;
}

void URPGGameInstance::SaveCharacterTypeToSpawn(ECharacterType Type)
{
	CharacterType = Type;
}

void URPGGameInstance::SaveCharacterTypeToSpawn(const FString& ID, ECharacterType Type)
{
	PlayersCharacterTypeMap.Add(ID, Type);
}

ECharacterType URPGGameInstance::GetPlayersCharacterType(const FString& ID)
{
	ECharacterType* Type = PlayersCharacterTypeMap.Find(ID);
	if (Type)
	{
		return *Type;
	}
	ELOG(TEXT("No Player ID"));
	return ECharacterType::ECT_Warrior;
}

void URPGGameInstance::Host()
{
	GetWorld()->ServerTravel("/Game/_Assets/Maps/MainLevel.MainLevel?listen");
}

void URPGGameInstance::Join()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	PlayerController->ClientTravel(FString(TEXT("127.0.0.1")), ETravelType::TRAVEL_Absolute);
}

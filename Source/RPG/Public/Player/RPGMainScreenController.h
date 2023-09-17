
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Enums/CharacterType.h"
#include "RPGMainScreenController.generated.h"

/**
 * 
 */
UCLASS()
class RPG_API ARPGMainScreenController : public APlayerController
{
	GENERATED_BODY()

public:

	ARPGMainScreenController();

	virtual void PostInitializeComponents() override;

	void SaveUniqueID(const FString& NewUniqueID);

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	void CharacterSelected(ECharacterType Type);

	UFUNCTION(Server, Reliable)
	void DeliverCharacterTypeServer(ECharacterType Type);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY(Replicated)
	FString UniqueID;

};

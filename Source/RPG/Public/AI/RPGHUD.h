
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUD.generated.h"

/**
 * 
 */

class URPGGameplayInterface;
class ARPGBasePlayerCharacter;

UCLASS()
class RPG_API ARPGHUD : public AHUD
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

private:

	void DrawOverlay();

	UFUNCTION()
	void SetHealthBarPercentage(float Percentage);

	UFUNCTION()
	void SetManaBarPercentage(float Percentage);

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> GameplayInterfaceClass;

	UPROPERTY()
	URPGGameplayInterface* GameplayInterface;

	ARPGBasePlayerCharacter* PlayerPawn;
};

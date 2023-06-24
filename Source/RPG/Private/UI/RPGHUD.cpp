

#include "UI/RPGHUD.h"
#include "UI/RPGGameplayInterface.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPG.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void ARPGHUD::BeginPlay()
{
	if (GetOwningPlayerController())
	{
		PlayerPawn = Cast<ARPGBasePlayerCharacter>(GetOwningPlayerController()->GetPawn());
	}

	if (PlayerPawn)
	{
		PlayerPawn->DOnChangeHealthPercentage.AddUFunction(this, FName("SetHealthBarPercentage"));
	}

	DrawOverlay();
}

void ARPGHUD::DrawOverlay()
{
	if (GameplayInterfaceClass)
	{
		GameplayInterface = CreateWidget<URPGGameplayInterface>(GetOwningPlayerController(), GameplayInterfaceClass);
		GameplayInterface->AddToViewport();
	}
}

void ARPGHUD::SetHealthBarPercentage(float Percentage)
{
	GameplayInterface->HealthBar->SetPercent(Percentage);
}

void ARPGHUD::SetManaBarPercentage(float Percentage)
{

}

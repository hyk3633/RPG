

#include "AI/RPGHUD.h"
#include "UI/RPGGameplayInterface.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPG.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

void ARPGHUD::BeginPlay()
{
	if (GetOwningPlayerController())
		PlayerPawn = Cast<ARPGBasePlayerCharacter>(GetOwningPlayerController()->GetPawn());
	else ELOG(TEXT("No Controller"));

	if (PlayerPawn)
		PlayerPawn->DOnChangeHealthPercentage.AddUFunction(this, FName("SetHealthBarPercentage"));
	else ELOG(TEXT("PlayerPawn Cast Failed!"));

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

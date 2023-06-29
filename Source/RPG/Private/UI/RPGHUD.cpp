
#include "UI/RPGHUD.h"
#include "UI/RPGGameplayInterface.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPG.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

ARPGHUD::ARPGHUD()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Obj_CooldownProgress(TEXT("/Game/_Assets/Materials/Circular/MI_ClockProgress.MI_ClockProgress"));
	if (Obj_CooldownProgress.Succeeded()) ClockProgressMatInst = Obj_CooldownProgress.Object;
}

void ARPGHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	DrawOverlay();
}

void ARPGHUD::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGHUD::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCooldownProgress();
}

void ARPGHUD::InitHUD()
{
	if (GetOwningPlayerController())
	{
		PlayerPawn = Cast<ARPGBasePlayerCharacter>(GetOwningPlayerController()->GetPawn());
	}

	if (PlayerPawn)
	{
		PlayerPawn->DOnChangeHealthPercentage.AddUFunction(this, FName("SetHealthBarPercentage"));
		PlayerPawn->DOnChangeManaPercentage.AddUFunction(this, FName("SetManaBarPercentage"));
		PlayerPawn->DOnAbilityCooldownEnd.AddUFunction(this, FName("CooldownProgressSetFull"));
	}
	SetHealthBarPercentage(1);
	SetManaBarPercentage(1);
	GameplayInterface->SetVisibility(ESlateVisibility::Visible);
}

void ARPGHUD::DrawOverlay()
{
	if (GameplayInterfaceClass)
	{
		GameplayInterface = CreateWidget<URPGGameplayInterface>(GetOwningPlayerController(), GameplayInterfaceClass);
		GameplayInterface->SetVisibility(ESlateVisibility::Hidden);
		GameplayInterface->AddToViewport();
	}
}

void ARPGHUD::SetHealthBarPercentage(float Percentage)
{
	GameplayInterface->HealthBar->SetPercent(Percentage);
	if (Percentage == 0.f)
	{
		GetWorldTimerManager().SetTimer(OffTimer, this, &ARPGHUD::OffHUD, 3.f);
	}
}

void ARPGHUD::SetManaBarPercentage(float Percentage)
{
	GameplayInterface->ManaBar->SetPercent(Percentage);
}

void ARPGHUD::UpdateCooldownProgress()
{
	if (PlayerPawn && PlayerPawn->IsLocallyControlled() && PlayerPawn->GetAbilityCooldownBit())
	{
		for (int8 idx = 0; idx < 4; idx++)
		{
			SetProgressPercentage(idx, PlayerPawn->GetCooldownPercentage(idx));
		}
	}
}

void ARPGHUD::SetProgressPercentage(const int8 Index, const float Percentage)
{
	ClockProgressMatInstDynamic = UMaterialInstanceDynamic::Create(ClockProgressMatInst, this);
	ClockProgressMatInstDynamic->SetScalarParameterValue(FName("Percent"), Percentage);

	switch (Index)
	{
	case 0:
		GameplayInterface->ClockProgress_Q->SetBrushResourceObject(ClockProgressMatInstDynamic);
		break;
	case 1:
		GameplayInterface->ClockProgress_W->SetBrushResourceObject(ClockProgressMatInstDynamic);
		break;
	case 2:
		GameplayInterface->ClockProgress_E->SetBrushResourceObject(ClockProgressMatInstDynamic);
		break;
	case 3:
		GameplayInterface->ClockProgress_R->SetBrushResourceObject(ClockProgressMatInstDynamic);
		break;
	}
}

void ARPGHUD::CooldownProgressSetFull(uint8 Bit)
{
	SetProgressPercentage(Bit, 1);
}

void ARPGHUD::OffHUD()
{
	GameplayInterface->SetVisibility(ESlateVisibility::Hidden);
}

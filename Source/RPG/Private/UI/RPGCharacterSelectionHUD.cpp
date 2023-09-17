
#include "UI/RPGCharacterSelectionHUD.h"
#include "UI/RPGCharacterSelectionInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/Button.h"

ARPGCharacterSelectionHUD::ARPGCharacterSelectionHUD()
{
	static ConstructorHelpers::FClassFinder<URPGCharacterSelectionInterface> WBP_CharacterSelect(TEXT("WidgetBlueprint'/Game/_Assets/Blueprints/HUD/WBP_CharacterSelect.WBP_CharacterSelect_C'"));
	if (WBP_CharacterSelect.Succeeded()) CharacterSelectWBPClass = WBP_CharacterSelect.Class;
}

void ARPGCharacterSelectionHUD::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterSelectWBPClass)
	{
		CharacterSelectionInterface = CreateWidget<URPGCharacterSelectionInterface>(GetOwningPlayerController(), CharacterSelectWBPClass);

		CharacterSelectionInterface->Button_Warrior->OnClicked.AddDynamic(this, &ARPGCharacterSelectionHUD::OnWarriorSelected);
		CharacterSelectionInterface->Button_Sorcerer->OnClicked.AddDynamic(this, &ARPGCharacterSelectionHUD::OnSorcererSelected);

		CharacterSelectionInterface->AddToViewport();
	}
}

void ARPGCharacterSelectionHUD::OnWarriorSelected()
{
	DOnCharacterSelected.Broadcast(ECharacterType::ECT_Warrior);
}

void ARPGCharacterSelectionHUD::OnSorcererSelected()
{
	DOnCharacterSelected.Broadcast(ECharacterType::ECT_Sorcerer);
}


#include "GameMode/RPGMainScreenGameMode.h"
#include "Player/RPGMainScreenController.h"
#include "UI/RPGCharacterSelectionHUD.h"
#include "UI/RPGCharacterSelectionInterface.h"
#include "GameInstance/RPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../RPG.h"

ARPGMainScreenGameMode::ARPGMainScreenGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_CharacterSelect(TEXT("WidgetBlueprint'/Game/_Assets/Blueprints/HUD/WBP_CharacterSelect.WBP_CharacterSelect_C'"));
	if (WBP_CharacterSelect.Succeeded()) CharacterSelectWBPClass = WBP_CharacterSelect.Class;

	DefaultPawnClass = nullptr;
	HUDClass = ARPGCharacterSelectionHUD::StaticClass();
	PlayerControllerClass = ARPGMainScreenController::StaticClass();
}

APlayerController* ARPGMainScreenGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// 클라이언트가 로그인 하면 클라이언트의 고유 ID를 클라이언트의 컨트롤러에 저장
	APlayerController* NewController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	ARPGMainScreenController* MainScreenController = Cast<ARPGMainScreenController>(NewController);
	if (MainScreenController)
	{
		MainScreenController->SaveUniqueID(UniqueId.ToString());
	}
	return NewController;
}

void ARPGMainScreenGameMode::ConnectToMainMap()
{
	UGameplayStatics::OpenLevel(this, TEXT("127.0.0.1"));
}

void ARPGMainScreenGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterSelectWBPClass)
	{
		CharacterSelectionInterface = CreateWidget<URPGCharacterSelectionInterface>(GetWorld(), CharacterSelectWBPClass);
		CharacterSelectionInterface->AddToViewport();
	}
}

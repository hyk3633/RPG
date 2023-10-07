
#include "Player/RPGMainScreenController.h"
#include "GameMode/RPGMainScreenGameMode.h"
#include "UI/RPGCharacterSelectionHUD.h"
#include "GameInstance/RPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "../RPG.h"

ARPGMainScreenController::ARPGMainScreenController()
{
	bShowMouseCursor = true;
}

void ARPGMainScreenController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FInputModeUIOnly UIInputMode;
	SetInputMode(UIInputMode);
}

void ARPGMainScreenController::SaveUniqueID(const FString& NewUniqueID)
{
	// 접속한 클라이언트의 고유 ID값을 서버의 플레이어 컨트롤러에 저장
	UniqueID = NewUniqueID;
}

void ARPGMainScreenController::BeginPlay()
{
	ARPGCharacterSelectionHUD* CharacterSelectHUD = Cast<ARPGCharacterSelectionHUD>(GetHUD());
	if (CharacterSelectHUD)
	{
		CharacterSelectHUD->DOnCharacterSelected.AddUFunction(this, FName("CharacterSelected"));
	}
}

void ARPGMainScreenController::CharacterSelected(ECharacterType Type)
{
	// 클라이언트면 플레이어가 선택한 캐릭터 타입 서버로 전달
	if (!HasAuthority())
	{
		DeliverCharacterTypeServer(Type);
	}
}

void ARPGMainScreenController::DeliverCharacterTypeServer_Implementation(ECharacterType Type)
{
	// 고유 ID를 키, 캐릭터 타입을 값으로 하는 서버의 게임 인스턴스의 맵에 저장
	GetWorld()->GetGameInstance<URPGGameInstance>()->SaveCharacterTypeToSpawn(UniqueID, Type);
	// 게임 모드에서 Open Level 호출
	GetWorld()->GetAuthGameMode<ARPGMainScreenGameMode>()->ConnectToMainMap();
}

void ARPGMainScreenController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGMainScreenController, UniqueID);
}

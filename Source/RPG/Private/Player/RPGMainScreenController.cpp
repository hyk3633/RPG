
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
	// ������ Ŭ���̾�Ʈ�� ���� ID���� ������ �÷��̾� ��Ʈ�ѷ��� ����
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
	// Ŭ���̾�Ʈ�� �÷��̾ ������ ĳ���� Ÿ�� ������ ����
	if (!HasAuthority())
	{
		DeliverCharacterTypeServer(Type);
		//ClientTravel(FString(TEXT("/Game/_Assets/Maps/MainLevel")), ETravelType::TRAVEL_Absolute);
		//UGameplayStatics::OpenLevel(this, TEXT("127.0.0.1"));
	}
}

void ARPGMainScreenController::DeliverCharacterTypeServer_Implementation(ECharacterType Type)
{
	// ���� ID�� Ű, ĳ���� Ÿ���� ������ �ϴ� ������ ���� �ν��Ͻ��� �ʿ� ����
	// ���� ��忡�� Open Level ȣ��
	//GetWorld()->GetAuthGameMode<ARPGMainScreenGameMode>()->ConnectToMainMap();
	GetWorld()->ServerTravel(FString(TEXT("/Game/_Assets/Maps/MainLevel")));
}

void ARPGMainScreenController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGMainScreenController, UniqueID);
}

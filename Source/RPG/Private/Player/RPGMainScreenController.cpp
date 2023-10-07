
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
	}
}

void ARPGMainScreenController::DeliverCharacterTypeServer_Implementation(ECharacterType Type)
{
	// ���� ID�� Ű, ĳ���� Ÿ���� ������ �ϴ� ������ ���� �ν��Ͻ��� �ʿ� ����
	GetWorld()->GetGameInstance<URPGGameInstance>()->SaveCharacterTypeToSpawn(UniqueID, Type);
	// ���� ��忡�� Open Level ȣ��
	GetWorld()->GetAuthGameMode<ARPGMainScreenGameMode>()->ConnectToMainMap();
}

void ARPGMainScreenController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGMainScreenController, UniqueID);
}

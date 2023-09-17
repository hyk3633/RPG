
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
	// �����̸� Ÿ�� ���� �� ���� ü����
	if (HasAuthority())
	{
		GetWorld()->GetGameInstance<URPGGameInstance>()->SaveCharacterTypeToSpawn(Type);
		GetWorld()->GetGameInstance<URPGGameInstance>()->Host();
	}
	else
	{
		// Ŭ���̾�Ʈ�� �÷��̾ ������ ĳ���� Ÿ�� ������ ����
		DeliverCharacterTypeServer(Type);
	}
}

void ARPGMainScreenController::DeliverCharacterTypeServer_Implementation(ECharacterType Type)
{
	// ���� ID�� Ű, ĳ���� Ÿ���� ������ �ϴ� ������ ���� �ν��Ͻ��� �ʿ� ����
	GetWorld()->GetGameInstance<URPGGameInstance>()->SaveCharacterTypeToSpawn(UniqueID, Type);
}

void ARPGMainScreenController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGMainScreenController, UniqueID);
}

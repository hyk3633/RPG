

#include "RPGGameModeBase.h"
#include "GameSystem/WorldGridManagerComponent.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "GameSystem/EnemyPooler.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/RPGPlayerController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/Boss/RPGBossEnemyCharacter.h"
#include "GameInstance/RPGGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "../RPG.h"

ARPGGameModeBase::ARPGGameModeBase()
{
	DefaultPawnClass = nullptr;

	WorldGridManager = CreateDefaultSubobject<UWorldGridManagerComponent>(TEXT("World Grid Manager"));

	ItemSpawnManager = CreateDefaultSubobject<UItemSpawnManagerComponent>(TEXT("Item Spawn Manager"));

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyInfo.DT_EnemyInfo"));
	if (Obj_EnemyInfoDataTable.Succeeded()) EnemyInfoDataTable = Obj_EnemyInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_EnemyAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_EnemyAssets.DT_EnemyAssets"));
	if (Obj_EnemyAssetsDataTable.Succeeded()) EnemyAssetsDataTable = Obj_EnemyAssetsDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileInfo.DT_ProjectileInfo"));
	if (Obj_ProjectileInfoDataTable.Succeeded()) ProjectileInfoDataTable = Obj_ProjectileInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileAssets.DT_ProjectileAssets"));
	if (Obj_ProjectileAssetsDataTable.Succeeded()) ProjectileAssetsDataTable = Obj_ProjectileAssetsDataTable.Object;

	static ConstructorHelpers::FClassFinder<ARPGBasePlayerCharacter> BP_WarriorCharacter(TEXT("Blueprint'/Game/_Assets/Blueprints/Player/BP_War.BP_War_C'"));
	if (BP_WarriorCharacter.Succeeded()) WarriorCharacterClass = BP_WarriorCharacter.Class;

	static ConstructorHelpers::FClassFinder<ARPGBasePlayerCharacter> BP_SorcererCharacter(TEXT("Blueprint'/Game/_Assets/Blueprints/Player/BP_Sor.BP_Sor_C'"));
	if (BP_SorcererCharacter.Succeeded()) SorcererCharacterClass = BP_SorcererCharacter.Class;

	bUseSeamlessTravel = true;
}

APlayerController* ARPGGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// ���� ��ȯ �� ������ Ŭ���̾�Ʈ�� ���� ID�� �ش� �÷��̾� ��Ʈ�ѷ��� ����
	APlayerController* NewController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(NewController);
	if (RPGController)
	{
		RPGController->SaveUniqueID(UniqueId.ToString());
	}
	return NewController;
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

}

void ARPGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SpawnPlayerCharacterAndPossess(NewPlayer);
}

void ARPGGameModeBase::SpawnPlayerCharacterAndPossess(APlayerController* Player)
{
	// ��� �÷��̾� ��Ʈ�ѷ� ã��
	TArray<AActor*> Players, Starts;
	UGameplayStatics::GetAllActorsOfClass(this, ARPGPlayerController::StaticClass(), Players);
	for (int8 Idx = 0; Idx < Players.Num(); Idx++)
	{
		// ������ ��� ��Ʈ�ѷ� �� ��� ������ �Ǵ� �������� ��Ʈ�ѷ��� ��ġ�ϴ� ��Ʈ�ѷ� ã��
		if (Players[Idx] == Player)
		{
			// �ش� ��Ʈ�ѷ��� �ε����� �±׷� ���� �÷��̾� ��ŸƮ ã��
			UGameplayStatics::GetAllActorsOfClassWithTag(this, APlayerStart::StaticClass(), FName(*FString::Printf(TEXT("%d"), Idx)), Starts);
			FTransform SpawnTransform;
			if (Starts.Num())
			{
				SpawnTransform = Starts[0]->GetActorTransform();
			}
			else
			{
				UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Starts);
				const int8 RandIdx = FMath::RandRange(0, Starts.Num() - 1);
				SpawnTransform = Starts[RandIdx]->GetActorTransform();
			}

			ARPGPlayerController* PlayerController = Cast<ARPGPlayerController>(Player);
			if (PlayerController == nullptr) break;

			ECharacterType CharacterType;
			if (Idx == 0) // ���� ��Ʈ�ѷ��̸�
			{
				CharacterType = PlayerController->GetCharacterType();
			}
			else // Ŭ���̾�Ʈ�� ��Ʈ�ѷ��̸� �ش� �÷��̾��� ���� ID�� ����� ĳ���� Ÿ���� ���� �ν��Ͻ����� ã��
			{
				const FString& PlayerID = PlayerController->GetUniqueID();
				CharacterType = GetWorld()->GetGameInstance<URPGGameInstance>()->GetPlayersCharacterType(PlayerID);
			}

			// ĳ���� Ÿ�Կ� �´� ĳ���� ���� �� ����
			ARPGBasePlayerCharacter* PlyerCharacter = nullptr;
			if (CharacterType == ECharacterType::ECT_Warrior && WarriorCharacterClass)
			{
				PlyerCharacter = GetWorld()->SpawnActor<ARPGBasePlayerCharacter>(WarriorCharacterClass, Starts[0]->GetActorTransform());
			}
			else if (CharacterType == ECharacterType::ECT_Sorcerer && SorcererCharacterClass)
			{
				PlyerCharacter = GetWorld()->SpawnActor<ARPGBasePlayerCharacter>(SorcererCharacterClass, Starts[0]->GetActorTransform());
			}
			if (PlyerCharacter)
			{
				PlayerController->Possess(PlyerCharacter);
			}
		}
		break;
	}
}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->AStarPlayer(Start, Dest, PathToDest);
	}
	else
	{
		ELOG(TEXT("Navigation is disable!"));
	}
}

void ARPGGameModeBase::SpawnItems(const FVector& Location)
{
	ItemSpawnManager->SpawnItems(Location);
}

void ARPGGameModeBase::DropItem(const FItemInfo& Info, const FVector& Location)
{
	ItemSpawnManager->DropItem(Info, Location);
}

FEnemyInfo* ARPGGameModeBase::GetEnemyInfo(const EEnemyType Type)
{
	if (EnemyInfoDataTable)
	{
		FEnemyInfo* Data = EnemyInfoDataTable->FindRow<FEnemyInfo>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

FEnemyAssets* ARPGGameModeBase::GetEnemyAssets(const EEnemyType Type)
{
	if (EnemyAssetsDataTable)
	{
		FEnemyAssets* Data = EnemyAssetsDataTable->FindRow<FEnemyAssets>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

FProjectileInfo* ARPGGameModeBase::GetProjectileInfo(const EProjectileType Type)
{
	if (ProjectileInfoDataTable)
	{
		FProjectileInfo* Data = ProjectileInfoDataTable->FindRow<FProjectileInfo>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}

FProjectileAssets* ARPGGameModeBase::GetProjectileAssets(const EProjectileType Type)
{
	if (ProjectileAssetsDataTable)
	{
		FProjectileAssets* Data = ProjectileAssetsDataTable->FindRow<FProjectileAssets>(*FString::FromInt(StaticCast<int8>(Type)), TEXT(""));
		if (Data) return Data;
	}
	return nullptr;
}


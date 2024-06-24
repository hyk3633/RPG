

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

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileInfoDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileInfo2.DT_ProjectileInfo2"));
	if (Obj_ProjectileInfoDataTable.Succeeded()) ProjectileInfoDataTable = Obj_ProjectileInfoDataTable.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> Obj_ProjectileAssetsDataTable(TEXT("/Game/_Assets/DataTable/DT_ProjectileAssets.DT_ProjectileAssets"));
	if (Obj_ProjectileAssetsDataTable.Succeeded()) ProjectileAssetsDataTable = Obj_ProjectileAssetsDataTable.Object;

	static ConstructorHelpers::FClassFinder<ARPGBasePlayerCharacter> BP_WarriorCharacter(TEXT("Blueprint'/Game/_Assets/Blueprints/Player/BP_War.BP_War_C'"));
	if (BP_WarriorCharacter.Succeeded()) WarriorCharacterClass = BP_WarriorCharacter.Class;

	static ConstructorHelpers::FClassFinder<ARPGBasePlayerCharacter> BP_SorcererCharacter(TEXT("Blueprint'/Game/_Assets/Blueprints/Player/BP_Sor.BP_Sor_C'"));
	if (BP_SorcererCharacter.Succeeded()) SorcererCharacterClass = BP_SorcererCharacter.Class;

	bUseSeamlessTravel = true;
}

void ARPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	ItemSpawnManager->InitItemSpawnManager();
}

void ARPGGameModeBase::SpawnPlayerCharacterAndPossess(APlayerController* Player, ECharacterType Type)
{
	const AActor* StartActor = ChoosePlayerStart(Player);
	if (StartActor == nullptr)
	{
		ELOG(TEXT("There is no place to spawn!!"));
		return;
	}

	// 캐릭터 타입에 맞는 캐릭터 스폰 후 빙의
	ARPGBasePlayerCharacter* PlayerCharacter = nullptr;
	if (Type == ECharacterType::ECT_Warrior && WarriorCharacterClass)
	{
		PlayerCharacter = GetWorld()->SpawnActor<ARPGBasePlayerCharacter>(WarriorCharacterClass, StartActor->GetActorTransform());
	}
	else if (Type == ECharacterType::ECT_Sorcerer && SorcererCharacterClass)
	{
		PlayerCharacter = GetWorld()->SpawnActor<ARPGBasePlayerCharacter>(SorcererCharacterClass, StartActor->GetActorTransform());
	}
	if (PlayerCharacter)
	{
		PlayerCharacter->SetCharacterType(Type);
		Player->Possess(PlayerCharacter);
	}
}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->GetAStarPath(Start, Dest, PathToDest);
	}
	else
	{
		ELOG(TEXT("Navigation is disable!"));
	}
}

void ARPGGameModeBase::GetPathToDestination(const FVector& Start, const FVector& Dest, TArray<FPos>& PathToDest, TArray<int32>& GridIndexArr)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->GetAStarPath(Start, Dest, PathToDest, GridIndexArr);
	}
	else
	{
		ELOG(TEXT("Navigation is disable!"));
	}
}

void ARPGGameModeBase::ClearEnemiesPathCost(TArray<int32>& GridIndexArr)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->ClearEnemiesPathCost(GridIndexArr);
	}
}

void ARPGGameModeBase::SetGridToImpassable(const FPos& _Pos)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->SetGridPassability(_Pos, false);
	}
}

void ARPGGameModeBase::SetGridToPassable(const FPos& _Pos)
{
	if (WorldGridManager->GetIsNavigationEnable())
	{
		WorldGridManager->SetGridPassability(_Pos, true);
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

void ARPGGameModeBase::DrawScore(const FVector& Location)
{
	WorldGridManager->DrawScore(Location);
}


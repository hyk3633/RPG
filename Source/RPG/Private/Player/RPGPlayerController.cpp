
#include "Player/RPGPlayerController.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/RPGPlayerState.h"
#include "Item/RPGItem.h"
#include "UI/RPGHUD.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Enums/PressedKey.h"
#include "GameFramework/HUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"

ARPGPlayerController::ARPGPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Obj_DefaultContext (TEXT("/Game/_Assets/Input/IMC_DefaulnputMappingContext.IMC_DefaulnputMappingContext"));
	if (Obj_DefaultContext.Succeeded()) DefaultMappingContext = Obj_DefaultContext.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_LeftClick (TEXT("/Game/_Assets/Input/IA_LeftClick.IA_LeftClick"));
	if (Obj_LeftClick.Succeeded()) LeftClickAction = Obj_LeftClick.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_RightClick (TEXT("/Game/_Assets/Input/IA_RightClick.IA_RightClick"));
	if (Obj_RightClick.Succeeded()) RightClickAction = Obj_RightClick.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_Q (TEXT("/Game/_Assets/Input/IA_Q.IA_Q"));
	if (Obj_Q.Succeeded()) QPressedAction = Obj_Q.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_W (TEXT("/Game/_Assets/Input/IA_W.IA_W"));
	if (Obj_W.Succeeded()) WPressedAction = Obj_W.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_E (TEXT("/Game/_Assets/Input/IA_E.IA_E"));
	if (Obj_E.Succeeded()) EPressedAction = Obj_E.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_R (TEXT("/Game/_Assets/Input/IA_R.IA_R"));
	if (Obj_R.Succeeded()) RPressedAction = Obj_R.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_I(TEXT("/Game/_Assets/Input/IA_I.IA_I"));
	if (Obj_I.Succeeded()) IPressedAction = Obj_I.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_MouseWheelScroll (TEXT("/Game/_Assets/Input/IA_ZoomInOut.IA_ZoomInOut"));
	if (Obj_MouseWheelScroll.Succeeded()) MouseWheelScroll = Obj_MouseWheelScroll.Object;
}

void ARPGPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	
	/*int32 GridSize;
	int32 GridDist;
	float WorldOffset;

	GridSize = 80;
	GridDist = 25;
	WorldOffset = ((GridSize * GridDist) / 2.f) - (GridDist / 2.f);

	for (int i = 0; i < GridSize; i++)
	{
		for (int j = 0; j < GridSize; j++)
		{
			float X = (GridDist * j) - FMath::TruncToInt(WorldOffset);
			float Y = (GridDist * i) - FMath::TruncToInt(WorldOffset);

			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 10.f, FColor::Green, true);
		}
	}

	float X = GetPawn()->GetActorLocation().X;
	float Y = GetPawn()->GetActorLocation().Y;
	int32 j = FMath::Floor(((X + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	int32 i = FMath::Floor(((Y + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	DrawDebugPoint(GetWorld(), FVector((GridDist * j) - FMath::TruncToInt(WorldOffset), (GridDist * i) - FMath::TruncToInt(WorldOffset), 10.f), 10.f, FColor::Red, true);*/
}

void ARPGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyCharacter = Cast<ARPGBasePlayerCharacter>(InPawn);
}

void ARPGPlayerController::OnRep_MyCharacter()
{
	if (RPGHUD == nullptr)
	{
		ARPGHUD* TempHUD = Cast<ARPGHUD>(GetHUD());
		if (TempHUD)
		{
			RPGHUD = TempHUD;
			RPGHUD->InitHUD();
		}
		else
		{
			WLOG(TEXT("Character cast is Failed!"));
		}
	}
	else
	{
		RPGHUD->ReloadHUD();
	}

	if (MyCharacter)
	{
		MyCharacter->ResetHealthManaUI();
	}
}

void ARPGPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyCharacter)
	{
		ItemTrace();
	}
}

void ARPGPlayerController::ItemTrace()
{
	if (IsValid(TracedItem))
	{
		TracedItem->SetItemNameTagVisibility(false);
		TracedItem->SetRenderCustomDepthOff();
	}
	if (MyCharacter->GetAiming())
	{
		TracedItem = nullptr;
		return;
	}

	FHitResult ItemTraceHit;
	GetHitResultUnderCursor(ECC_ItemTrace, false, ItemTraceHit);

	if (ItemTraceHit.bBlockingHit)
	{
		TracedItem = Cast<ARPGItem>(ItemTraceHit.GetActor());
		if (IsValid(TracedItem))
		{
			TracedItem->SetItemNameTagVisibility(true);
			TracedItem->SetRenderCustomDepthOn(200);
		}
	}
	else
	{
		TracedItem = nullptr;
	}
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ARPGPlayerController::LeftClickAction_StopMove);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::LeftClickAction_SetPath);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::RightClick_AttackOrSetAbilityPoint);
		EnhancedInputComponent->BindAction(QPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::QPressedAction_Cast);
		EnhancedInputComponent->BindAction(WPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::WPressedAction_Cast);
		EnhancedInputComponent->BindAction(EPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::EPressedAction_Cast);
		EnhancedInputComponent->BindAction(RPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::RPressedAction_Cast);
		EnhancedInputComponent->BindAction(IPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::IPressedAction_ToggleInventory);
		EnhancedInputComponent->BindAction(MouseWheelScroll, ETriggerEvent::Triggered, this, &ARPGPlayerController::MouseWheelScroll_ZoomInOut);
	}
}

void ARPGPlayerController::LeftClickAction_StopMove()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->StopMove();
}

void ARPGPlayerController::LeftClickAction_SetPath()
{
	if (MyCharacter == nullptr) return;
	if (MyCharacter->GetAbilityERMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (IsValid(TracedItem) && TracedItem->GetDistanceTo(MyCharacter) < 500.f)
	{
		PickupItemServer(TracedItem);
	}
	else if(bIsInventoryOn == false)
	{
		MyCharacter->SetDestinationAndPath();
	}
}

void ARPGPlayerController::PickupItemServer_Implementation(ARPGItem* Item)
{
	PickupItem(Item);
}

void ARPGPlayerController::PickupItem(ARPGItem* Item)
{
	const int32 Num = GetPlayerState<ARPGPlayerState>()->AddItem(Item);

	const EItemType Type = Item->GetItemInfo().ItemType;
	switch (Type)
	{
	case EItemType::EIT_Coin:
		PickupCoinsClient(GetPlayerState<ARPGPlayerState>()->GetCoins());
		break;
	case EItemType::EIT_HealthPotion:
		PickupPotionClient(Num, Type, GetPlayerState<ARPGPlayerState>()->GetHealthPotionCount());
		break;
	case EItemType::EIT_ManaPotion:
		PickupPotionClient(Num, Type, GetPlayerState<ARPGPlayerState>()->GetManaPotionCount());
		break;
	case EItemType::EIT_Armour:
	case EItemType::EIT_Accessories:
		PickupEquipmentClient(Num, Type);
		break;
	}

	Item->DestroyFromAllClients();
}

void ARPGPlayerController::PickupCoinsClient_Implementation(const int32 CoinAmount)
{
	if (RPGHUD)
	{
		const EItemType TracedItemType = TracedItem->GetItemInfo().ItemType;
		RPGHUD->AddCoins(CoinAmount);
	}
}

void ARPGPlayerController::PickupPotionClient_Implementation(const int32 UniqueNum, const EItemType PotionType, const int32 PotionCount)
{
	if (RPGHUD)
	{
		RPGHUD->AddPotion(UniqueNum, PotionType, PotionCount);
	}
}

void ARPGPlayerController::PickupEquipmentClient_Implementation(const int32 UniqueNum, const EItemType ItemType)
{
	if (RPGHUD)
	{
		RPGHUD->AddEquipment(UniqueNum, ItemType);
	}
}

void ARPGPlayerController::UseItem(const int32& UniqueNum)
{
	UseItemServer(UniqueNum);
}

void ARPGPlayerController::UseItemServer_Implementation(const int32 UniqueNum)
{
	GetPlayerState<ARPGPlayerState>()->UseItem(UniqueNum);
	UniqueNum == 0 ? MyCharacter->RecoveryHealth(200) : MyCharacter->RecoveryMana(200);
	UpdateItemInfoClient(UniqueNum, GetItemCount(UniqueNum));
}

int32 ARPGPlayerController::GetItemCount(const int32 UniqueNum)
{
	if (UniqueNum == 0)
	{
		return GetPlayerState<ARPGPlayerState>()->GetHealthPotionCount();
	}
	else if (UniqueNum == 1)
	{
		return GetPlayerState<ARPGPlayerState>()->GetManaPotionCount();
	}
	else
	{
		return 0;
	}
}

void ARPGPlayerController::UpdateItemInfoClient_Implementation(const int32 UniqueNum, const int32 ItemCount)
{
	if (RPGHUD == nullptr) return;

	if (UniqueNum == 0)
	{
		RPGHUD->UpdateItemCount(UniqueNum, ItemCount);
	}
	else if (UniqueNum == 1)
	{
		RPGHUD->UpdateItemCount(UniqueNum, ItemCount);
	}
	else
	{
		RPGHUD->UpdateItemCount(UniqueNum, 0);
	}
}

void ARPGPlayerController::EquipOrUnequipItem(const int32& UniqueNum)
{
	EquipOrUnequipItemServer(UniqueNum);
}

void ARPGPlayerController::EquipOrUnequipItemServer_Implementation(const int32 UniqueNum)
{
	// 플레이어 스테이트에 장착 아이템 정보 저장
	GetPlayerState<ARPGPlayerState>()->EquipOrUnequipItem(UniqueNum);

	// 플레이어 캐릭터의 스탯 반영 // 구조체 참조로 전달
	// Validate 함수 추가

}

void ARPGPlayerController::DiscardItem(const int32& UniqueNum)
{
	DiscardItemServer(UniqueNum);
}

void ARPGPlayerController::DiscardItemServer_Implementation(const int32 UniqueNum)
{
	// 아이템 스포너로 아이템 스폰
	FItemInfo ItemInfo;
	const bool bIsItemExist = GetPlayerState<ARPGPlayerState>()->GetItemInfo(UniqueNum, ItemInfo);
	if (bIsItemExist == false) return;

	FVector Location = MyCharacter->GetActorLocation();
	SpawnLocation.X += 50.f;
	SpawnLocation.Z += 100.f;
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->DropItem(ItemInfo, Location);

	// 플레이어 스테이트에서 아이템 제거
	GetPlayerState<ARPGPlayerState>()->DiscardItem(UniqueNum);

	UpdateItemInfoClient(UniqueNum, GetItemCount(UniqueNum));
}

void ARPGPlayerController::GetItemInfoStruct(const int32& UniqueNum)
{
	GetItemInfoStructServer(UniqueNum);
}

void ARPGPlayerController::GetItemInfoStructServer_Implementation(const int32 UniqueNum)
{
	FItemInfo ItemInfo;
	const bool bIsItemExist = GetPlayerState<ARPGPlayerState>()->GetItemInfo(UniqueNum, ItemInfo);
	if (bIsItemExist)
	{
		GetItemInfoStructClient(ItemInfo);
	}
}

void ARPGPlayerController::GetItemInfoStructClient_Implementation(const FItemInfo& Info)
{
	if (RPGHUD)
	{
		RPGHUD->ShowItemStatTextBox(Info);
	}
}

void ARPGPlayerController::RightClick_AttackOrSetAbilityPoint()
{
	if (bIsInventoryOn || MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->GetCursorHitResultCastAbility();
	}
	else
	{
		MyCharacter->DoNormalAttack();
	}
}

void ARPGPlayerController::QPressedAction_Cast()
{
	if (bIsInventoryOn || MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if(MyCharacter->IsAbilityAvailable(EPressedKey::EPK_Q))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_Q);
	}
}

void ARPGPlayerController::WPressedAction_Cast()
{
	if (bIsInventoryOn || MyCharacter == nullptr && MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_W))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_W);
	}
}

void ARPGPlayerController::EPressedAction_Cast()
{
	if (bIsInventoryOn || MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_E))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_E);
	}
}

void ARPGPlayerController::RPressedAction_Cast()
{
	if (bIsInventoryOn || MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_R))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_R);
	}
}

void ARPGPlayerController::IPressedAction_ToggleInventory()
{
	if (RPGHUD == nullptr || MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else
	{
		bIsInventoryOn = !bIsInventoryOn;
		RPGHUD->InventoryWidgetToggle(bIsInventoryOn);
	}
}

void ARPGPlayerController::MouseWheelScroll_ZoomInOut(const FInputActionValue& Value)
{
	if (MyCharacter == nullptr) return;

	MyCharacter->CameraZoomInOut(-Value.Get<float>());
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, MyCharacter);
}


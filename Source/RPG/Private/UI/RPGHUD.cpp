
#include "UI/RPGHUD.h"
#include "UI/RPGGameplayInterface.h"
#include "UI/RPGInventoryWidget.h"
#include "UI/RPGInventorySlotWidget.h"
#include "UI/RPGItemSlotMenuWidget.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "Player/RPGPlayerController.h"
#include "Item/RPGItem.h"
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "../RPG.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "UObject/ConstructorHelpers.h"

ARPGHUD::ARPGHUD()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> Obj_CooldownProgress(TEXT("/Game/_Assets/Materials/Circular/MI_ClockProgress.MI_ClockProgress"));
	if (Obj_CooldownProgress.Succeeded()) ClockProgressMatInst = Obj_CooldownProgress.Object;
}

void ARPGHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	
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
		DrawOverlay();
	}
	
	if (PlayerPawn)
	{
		PlayerPawn->DOnChangeHealthPercentage.AddUFunction(this, FName("SetHealthBarPercentage"));
		PlayerPawn->DOnChangeManaPercentage.AddUFunction(this, FName("SetManaBarPercentage"));
		PlayerPawn->DOnAbilityCooldownEnd.AddUFunction(this, FName("CooldownProgressSetFull"));
	}

	if (ItemSlotMenuClass)
	{
		ItemSlotMenuWidget = CreateWidget<URPGItemSlotMenuWidget>(GetOwningPlayerController(), ItemSlotMenuClass);
		ItemSlotMenuWidget->GetUseButton()->OnClicked.AddDynamic(this, &ARPGHUD::OnUseButtonClicked);
		ItemSlotMenuWidget->GetDiscardButton()->OnClicked.AddDynamic(this, &ARPGHUD::OnDiscardButtonClicked);
		ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		ItemSlotMenuWidget->AddToViewport();
	}

	SetHealthBarPercentage(1);
	SetManaBarPercentage(1);

	GameplayInterface->SetVisibility(ESlateVisibility::Visible);
}

void ARPGHUD::OffHUD()
{
	GameplayInterface->SetVisibility(ESlateVisibility::Hidden);
}

void ARPGHUD::DrawOverlay()
{
	if (GameplayInterfaceClass)
	{
		GameplayInterface = CreateWidget<URPGGameplayInterface>(GetOwningPlayerController(), GameplayInterfaceClass);
		GameplayInterface->SetVisibility(ESlateVisibility::Hidden);
		GameplayInterface->InventoryWidget->InitInventory();
		GameplayInterface->AddToViewport();
		InitInventorySlot();
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

/** ---------------------- 인벤토리 ---------------------- */

void ARPGHUD::InitInventorySlot()
{
	if (GameplayInterface->InventoryWidget == nullptr || ItemSlotClass == nullptr) return;

	// 슬롯 96개 미리 생성
	for (int32 Idx = 0; Idx < 96; Idx++)
	{
		URPGInventorySlotWidget* InvSlot = CreateWidget<URPGInventorySlotWidget>(GetOwningPlayerController(), ItemSlotClass);
		if (InvSlot)
		{
			// 16개만 활성화
			if (Idx < 16)
			{	
				GameplayInterface->InventoryWidget->AddSlotToGridPanel(InvSlot, Idx / 4, Idx % 4);
				InvSlot->SetSlotRowColumn(Idx / 4, Idx % 4);
				ActivadtedSlots.Add(InvSlot);
			}
			else
			{
				InvSlot->SetVisibility(ESlateVisibility::Hidden);
			}

			InvSlot->BindButtonEvent();
			InvSlot->DOnIconButtonClicked.AddUFunction(this, FName("OnItemSlotButtonClickEvent"));
			
			EmptySlots.Add(InvSlot);
		}
	}

	ActivatedItemSlotNum = 16;
}

void ARPGHUD::AddCoins(const int32 CoinAmount)
{
	if (GameplayInterface->InventoryWidget == nullptr) return;

	GameplayInterface->InventoryWidget->AddCoins(CoinAmount);
}

void ARPGHUD::AddPotion(const int32 UniqueNum, const EItemType ItemType, const int32 PotionCount)
{
	ExpandInventoryIfNoSpace();

	if (ItemSlotMap.Find(UniqueNum) == nullptr)
	{
		ItemSlotMap.Add(UniqueNum, ActivadtedSlots[SavedItemSlotCount]);
		ActivadtedSlots[SavedItemSlotCount]->SaveItemToSlot(ItemType);
		ActivadtedSlots[SavedItemSlotCount]->SetItemCountText(PotionCount);
		ActivadtedSlots[SavedItemSlotCount]->SetUniqueNumber(UniqueNum);
		SetSlotIcon(UniqueNum, ItemType);
		SavedItemSlotCount++;
	}
	else
	{
		(*ItemSlotMap.Find(UniqueNum))->SetItemCountText(PotionCount);
	}
}

void ARPGHUD::AddEquipment(const int32 UniqueNum, const EItemType ItemType)
{
	ExpandInventoryIfNoSpace();

	ItemSlotMap.Add(UniqueNum, ActivadtedSlots[SavedItemSlotCount]);
	ActivadtedSlots[SavedItemSlotCount]->SaveItemToSlot(ItemType);
	ActivadtedSlots[SavedItemSlotCount]->SetItemCountText(1);
	ActivadtedSlots[SavedItemSlotCount]->SetUniqueNumber(UniqueNum);
	SetSlotIcon(UniqueNum, ItemType);
	SavedItemSlotCount++;
}

void ARPGHUD::UpdatePotionCount(const int32 UniqueNum, const EItemType ItemType, const int32 PotionCount)
{
	if (PotionCount == 0)
	{
		SavedItemSlotCount--;

		ClearItemSlot(UniqueNum);

		// 슬롯 1 페이지(16개 슬롯)에서 한 개의 아이템만 저장되어 있는 경우
		// 페이지 전체 비활성화
		if(ActivatedItemSlotNum > 16 && SavedItemSlotCount % 16 == 0)
		{
			if (GameplayInterface->InventoryWidget == nullptr)
			{
				GameplayInterface->InventoryWidget->RemoveSlotPage();
			}
			// 배열 뒤에서부터 제거
			for (int32 Idx = ActivatedItemSlotNum - 1; Idx > ActivatedItemSlotNum - 16; Idx--)
			{
				ActivadtedSlots.RemoveAt(Idx);
			}
			ActivatedItemSlotNum -= 16;
		}
	}
	else
	{
		(*ItemSlotMap.Find(UniqueNum))->SetItemCountText(PotionCount);
	}
}

void ARPGHUD::ClearItemSlot(const int32 UniqueNum)
{
	// 슬롯 UI 초기화
	URPGInventorySlotWidget* SlotToRemove = (*ItemSlotMap.Find(UniqueNum));
	SlotToRemove->ClearSlot();

	// 맵에서 제거
	ItemSlotMap.Remove(UniqueNum);

	if (SavedItemSlotCount > 1)
	{
		// 활성화된 슬롯 뒤로 보내기
		int32 Index;
		ActivadtedSlots.Find(SlotToRemove, Index);
		for (int32 Idx = Index; Idx < SavedItemSlotCount; Idx++)
		{
			ActivadtedSlots[Idx] = ActivadtedSlots[Idx + 1];

		}
		ActivadtedSlots[SavedItemSlotCount] = SlotToRemove;

		// 그리드 재정렬
		if (GameplayInterface->InventoryWidget)
		{
			GameplayInterface->InventoryWidget->SortGridPanel(SlotToRemove, Index, SavedItemSlotCount);
		}
	}
}

void ARPGHUD::SetSlotIcon(const int32 UniqueNum, const EItemType ItemType)
{
	const int32 RowNumber = StaticCast<int32>(ItemType);
	FItemOptionTableRow* ItemTableRow = ItemDataTable->FindRow<FItemOptionTableRow>(FName(*(FString::FormatAsNumber(RowNumber))), FString(""));
	if (ItemTableRow && ItemTableRow->ItemIcon)
	{
		(*ItemSlotMap.Find(UniqueNum))->SetSlotIcon(ItemTableRow->ItemIcon);
	}
}

void ARPGHUD::ExpandInventoryIfNoSpace()
{
	if (GameplayInterface->InventoryWidget == nullptr) return;

	if (SavedItemSlotCount == ActivatedItemSlotNum)
	{
		if (ActivatedItemSlotNum == 96)
		{
			ELOG(TEXT("Inventory is full!"));
			return;
		}

		for (int32 Idx = ActivatedItemSlotNum; Idx < ActivatedItemSlotNum + 16; Idx++)
		{
			ActivadtedSlots.Add(EmptySlots[Idx]);
			EmptySlots[Idx]->SetSlotRowColumn(Idx / 4, Idx % 4);
			EmptySlots[Idx]->SetVisibility(ESlateVisibility::Visible);
			GameplayInterface->InventoryWidget->AddSlotToGridPanel(EmptySlots[Idx], Idx / 4, Idx % 4);
		}

		ActivatedItemSlotNum += 16;
	}
}

void ARPGHUD::OnItemSlotButtonClickEvent(int32 UniqueNum)
{
	bIsItemSlotMenuWidgetOn = true;
	SelectedItemUniqueNum = UniqueNum;

	float MX, MY;
	GetOwningPlayerController()->GetMousePosition(MX, MY);
	int32 VX, VY;
	GetOwningPlayerController()->GetViewportSize(VX, VY);

	if (UniqueNum < 2)
	{
		ItemSlotMenuWidget->SetUseText(FString(TEXT("사용하기")));
	}
	else
	{
		ItemSlotMenuWidget->SetUseText(FString(TEXT("장착하기")));
	}
	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Visible);
	ItemSlotMenuWidget->SetWidgetPosition(FVector2D(MX - (VX / 2), MY - (VY / 2)));
}

void ARPGHUD::OnUseButtonClicked()
{
	ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(GetOwningPlayerController());
	if (RPGController == nullptr) return;

	bIsItemSlotMenuWidgetOn = false;

	if (SelectedItemUniqueNum < 2)
	{
		RPGController->UseItem(SelectedItemUniqueNum);
	}
	else
	{
		RPGController->EquipItem(SelectedItemUniqueNum);
	}

	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ARPGHUD::OnDiscardButtonClicked()
{
	ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(GetOwningPlayerController());
	if (RPGController == nullptr) return;

	bIsItemSlotMenuWidgetOn = false;

	RPGController->DiscardItem(SelectedItemUniqueNum);

	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
}

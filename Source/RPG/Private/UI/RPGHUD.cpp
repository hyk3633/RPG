
#include "UI/RPGHUD.h"
#include "UI/RPGGameplayInterface.h"
#include "UI/RPGInventoryWidget.h"
#include "UI/RPGInventorySlotWidget.h"
#include "UI/RPGItemSlotMenuWidget.h"
#include "UI/RPGStatTextBoxWidget.h"
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
	CastPawnAndBindFunctions();

	if (ItemSlotMenuClass)
	{
		ItemSlotMenuWidget = CreateWidget<URPGItemSlotMenuWidget>(GetOwningPlayerController(), ItemSlotMenuClass);
		ItemSlotMenuWidget->GetUseButton()->OnClicked.AddDynamic(this, &ARPGHUD::OnUseOrEquipButtonClicked);
		ItemSlotMenuWidget->GetDiscardButton()->OnClicked.AddDynamic(this, &ARPGHUD::OnDiscardButtonClicked);
		ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		ItemSlotMenuWidget->AddToViewport();
	}

	if (ItemStatBoxClass)
	{
		ItemStatBoxWidget = CreateWidget<URPGStatTextBoxWidget>(GetOwningPlayerController(), ItemStatBoxClass);
		ItemStatBoxWidget->SetVisibility(ESlateVisibility::Hidden);
		ItemStatBoxWidget->AddToViewport();
	}

	DrawOverlay();
}

void ARPGHUD::CastPawnAndBindFunctions()
{
	if (GetOwningPlayerController())
	{
		PlayerPawn = Cast<ARPGBasePlayerCharacter>(GetOwningPlayerController()->GetPawn());
	}

	if (PlayerPawn)
	{
		PlayerPawn->DOnChangeHealthPercentage.AddUFunction(this, FName("SetHealthBarPercentage"));
		PlayerPawn->DOnChangeManaPercentage.AddUFunction(this, FName("SetManaBarPercentage"));
		PlayerPawn->DOnAbilityCooldownEnd.AddUFunction(this, FName("CooldownProgressSetFull"));
	}
}

void ARPGHUD::ReloadHUD()
{
	CastPawnAndBindFunctions();

	GameplayInterface->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
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
		GameplayInterface->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
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
				ActivadtedSlots.Add(InvSlot);
			}
			else
			{
				InvSlot->SetVisibility(ESlateVisibility::Hidden);
			}

			InvSlot->BindButtonEvent();
			InvSlot->DOnIconButtonClicked.AddUFunction(this, FName("OnItemSlotButtonClickEvent"));
			InvSlot->DOnIconButtonHovered.AddUFunction(this, FName("OnItemSlotButtonHoveredEvent"));
			
			EmptySlots.Add(InvSlot);
		}
	}

	ActivatedItemSlotNum = 16;
}

void ARPGHUD::InventoryWidgetToggle(const bool bInventoryOn)
{
	if (bInventoryOn)
	{
		GameplayInterface->InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		GameplayInterface->InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ARPGHUD::AddCoins(const int32& CoinAmount)
{
	if (GameplayInterface->InventoryWidget == nullptr) return;

	GameplayInterface->InventoryWidget->AddCoins(CoinAmount);
}

void ARPGHUD::AddPotion(const int32& UniqueNum, const EItemType ItemType, const int32& PotionCount)
{
	ExpandInventoryIfNoSpace();

	if (ItemSlotMap.Contains(UniqueNum) == false)
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

void ARPGHUD::AddEquipment(const int32& UniqueNum, const EItemType ItemType)
{
	ExpandInventoryIfNoSpace();

	ItemSlotMap.Add(UniqueNum, ActivadtedSlots[SavedItemSlotCount]);
	ActivadtedSlots[SavedItemSlotCount]->SaveItemToSlot(ItemType);
	ActivadtedSlots[SavedItemSlotCount]->SetItemCountText(1);
	ActivadtedSlots[SavedItemSlotCount]->SetUniqueNumber(UniqueNum);
	SetSlotIcon(UniqueNum, ItemType);
	SavedItemSlotCount++;
}

void ARPGHUD::UpdateItemCount(const int32& UniqueNum, const int32& ItemCount)
{
	if (ItemCount == 0)
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
		(*ItemSlotMap.Find(UniqueNum))->SetItemCountText(ItemCount);
	}
}

void ARPGHUD::ClearItemSlot(const int32& UniqueNum)
{
	// 슬롯 UI 초기화
	URPGInventorySlotWidget* SlotToRemove = (*ItemSlotMap.Find(UniqueNum));
	SlotToRemove->ClearSlot();

	// 맵에서 제거
	ItemSlotMap.Remove(UniqueNum);

	if (SavedItemSlotCount > 0)
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

void ARPGHUD::SetSlotIcon(const int32& UniqueNum, const EItemType ItemType)
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
			EmptySlots[Idx]->SetVisibility(ESlateVisibility::Visible);
			GameplayInterface->InventoryWidget->AddSlotToGridPanel(EmptySlots[Idx], Idx / 4, Idx % 4);
		}

		ActivatedItemSlotNum += 16;
	}
}

void ARPGHUD::OnItemSlotButtonClickEvent(const int32 UniqueNum)
{
	bIsItemSlotMenuWidgetOn = true;
	SelectedItemUniqueNum = UniqueNum;

	if (UniqueNum < 2)
	{
		ItemSlotMenuWidget->SetUseText(FString(TEXT("사용하기")));
	}
	else
	{
		if (UniqueNum == EquippedArmourUnieuqNum || UniqueNum == EquippedAccessoriesUnieuqNum)
		{
			ItemSlotMenuWidget->SetUseText(FString(TEXT("장착해제")));
		}
		else
		{
			ItemSlotMenuWidget->SetUseText(FString(TEXT("장착하기")));
		}
	}

	FVector2D DrawPosition;
	GetPositionUnderCursor(DrawPosition);
	ItemSlotMenuWidget->SetWidgetPosition(DrawPosition);
	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Visible);
}

void ARPGHUD::GetPositionUnderCursor(FVector2D& Position)
{
	float MX, MY;
	GetOwningPlayerController()->GetMousePosition(MX, MY);
	int32 VX, VY;
	GetOwningPlayerController()->GetViewportSize(VX, VY);

	Position.X = MX - (VX / 2);
	Position.Y = MY - (VY / 2);
}

void ARPGHUD::OnUseOrEquipButtonClicked()
{
	ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(GetOwningPlayerController());
	if (RPGController == nullptr) return;

	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	
	bIsItemSlotMenuWidgetOn = false;

	if (SelectedItemUniqueNum < 2)
	{
		RPGController->UseItem(SelectedItemUniqueNum);
	}
	else
	{
		RPGController->EquipOrUnequipItem(SelectedItemUniqueNum);
		EquipOrUnequipItem();
	}
}

void ARPGHUD::EquipOrUnequipItem()
{
	if (SelectedItemUniqueNum < 2) return;
	if (ItemSlotMap.Contains(SelectedItemUniqueNum) == false) return;

	URPGInventorySlotWidget* SelectedSlot = (*ItemSlotMap.Find(SelectedItemUniqueNum));
	const EItemType SelectedItemType = SelectedSlot->GetSavedItemType();

	// 장착된 아이템 장착 해제
	if (SelectedItemUniqueNum == EquippedArmourUnieuqNum || SelectedItemUniqueNum == EquippedAccessoriesUnieuqNum)
	{
		GameplayInterface->InventoryWidget->ClearEquipmentSlot(SelectedItemType);
		SelectedSlot->SetBorderStateToEquipped(false);
		if (SelectedItemType == EItemType::EIT_Armour)
		{
			EquippedArmourUnieuqNum = -1;
		}
		else if (SelectedItemType == EItemType::EIT_Accessories)
		{
			EquippedAccessoriesUnieuqNum = -1;
		}
	}
	else // 장비 창에 장착할 아이템의 아이콘으로 설정
	{
		GameplayInterface->InventoryWidget->SetEquipmentSlot(SelectedItemType, SelectedSlot->GetIconMaterial());
		SelectedSlot->SetBorderStateToEquipped(true);
		if (SelectedItemType == EItemType::EIT_Armour)
		{
			EquippedArmourUnieuqNum = SelectedItemUniqueNum;
		}
		else if (SelectedItemType == EItemType::EIT_Accessories)
		{
			EquippedAccessoriesUnieuqNum = SelectedItemUniqueNum;
		}
	}
}

void ARPGHUD::OnDiscardButtonClicked()
{
	ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(GetOwningPlayerController());
	if (RPGController == nullptr) return;

	ItemSlotMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	
	bIsItemSlotMenuWidgetOn = false;

	if (SelectedItemUniqueNum == EquippedArmourUnieuqNum || SelectedItemUniqueNum == EquippedAccessoriesUnieuqNum)
	{
		EquipOrUnequipItem();
	}

	RPGController->DiscardItem(SelectedItemUniqueNum);
}

void ARPGHUD::OnItemSlotButtonHoveredEvent(int32 UniqueNum)
{
	if (UniqueNum != -1)
	{
		ARPGPlayerController* RPGController = Cast<ARPGPlayerController>(GetOwningPlayerController());
		if (RPGController == nullptr) return;

		RPGController->GetItemInfoStruct(UniqueNum);
	}
	else
	{
		HideItemStatTextBox();
	}
}

void ARPGHUD::ShowItemStatTextBox(const FItemInfo& Info)
{
	const int32 RowNumber = StaticCast<int32>(Info.ItemType);
	FItemOptionTableRow* ItemTableRow = ItemDataTable->FindRow<FItemOptionTableRow>(FName(*(FString::FormatAsNumber(RowNumber))), FString(""));

	FString StatString;
	const int8 ArrEnd = ItemTableRow->PropertyNames.Num();
	for (int8 Idx = 0; Idx < ArrEnd; Idx++)
	{
		if (Info.ItemStatArr[Idx] > 0)
		{
			if (StatString.Len() > 0) StatString += FString(TEXT("\n"));
			StatString += FString::Printf(TEXT("%s : %.1f"), *ItemTableRow->PropertyNames[Idx], Info.ItemStatArr[Idx]);
		}
	}

	ItemStatBoxWidget->SetStatText(StatString);
	FVector2D DrawPosition;
	GetPositionUnderCursor(DrawPosition);
	ItemStatBoxWidget->SetWidgetPosition(DrawPosition);
	ItemStatBoxWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ARPGHUD::HideItemStatTextBox()
{
	ItemStatBoxWidget->SetVisibility(ESlateVisibility::Hidden);
}

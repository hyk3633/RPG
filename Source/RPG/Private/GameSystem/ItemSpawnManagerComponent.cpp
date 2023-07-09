
#include "GameSystem/ItemSpawnManagerComponent.h"
#include "Item/RPGItem.h"
#include "../RPG.h"
#include "UObject/ConstructorHelpers.h"

UItemSpawnManagerComponent::UItemSpawnManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UItemSpawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UItemSpawnManagerComponent::DropItem(const FItemInfo& Info, const FVector& Location)
{
	const int32 RowNumber = StaticCast<int32>(Info.ItemType);
	FItemOptionTableRow* ItemTableRow = ItemDataTable->FindRow<FItemOptionTableRow>(FName(*(FString::FormatAsNumber(RowNumber))), FString(""));

	ARPGItem* SpawnedItem = GetWorld()->SpawnActorDeferred<ARPGItem>(ItemClass, FTransform(FRotator::ZeroRotator, Location));
	if (SpawnedItem)
	{
		SpawnedItem->SetItemMesh(ItemTableRow->ItemMesh);
		SpawnedItem->SetItemInfo(Info);
		SpawnedItem->FinishSpawning(FTransform(FRotator::ZeroRotator, Location));
	}
}

void UItemSpawnManagerComponent::SpawnItems(const FVector& Location)
{
	int32 RandInt = FMath::RandRange(0, StaticCast<int32>(EItemType::EIT_MAX) - 1);
	const EItemType ItemType = StaticCast<EItemType>(RandInt);

	// 코인 무조건 스폰
	ItemInitializeBeforeSpawn(EItemType::EIT_Coin, GetRandomVector(Location));

	ItemInitializeBeforeSpawn(EItemType::EIT_HealthPotion, GetRandomVector(Location));
	ItemInitializeBeforeSpawn(EItemType::EIT_ManaPotion, GetRandomVector(Location));
	ItemInitializeBeforeSpawn(EItemType::EIT_Armour, GetRandomVector(Location));
	ItemInitializeBeforeSpawn(EItemType::EIT_Accessories, GetRandomVector(Location));

	if (FMath::RandRange(1, 100) < 30)
	{
		const int8 RandNum = FMath::RandRange(0, 1);
		if (RandNum)
		{
			ItemInitializeBeforeSpawn(EItemType::EIT_HealthPotion, GetRandomVector(Location));
		}
		else
		{
			ItemInitializeBeforeSpawn(EItemType::EIT_ManaPotion, GetRandomVector(Location));
		}
	}
	if (FMath::RandRange(1, 100) < 30)
	{
		const int8 RandNum = FMath::RandRange(0, 1);
		if (RandNum)
		{
			ItemInitializeBeforeSpawn(EItemType::EIT_Armour, GetRandomVector(Location));
		}
		else
		{
			ItemInitializeBeforeSpawn(EItemType::EIT_Accessories, GetRandomVector(Location));
		}
	}
}

FVector& UItemSpawnManagerComponent::GetRandomVector(FVector Vector)
{
	Vector.X += FMath::RandRange(-25, 25);
	Vector.Y += FMath::RandRange(-25, 25);

	return Vector;
}

void UItemSpawnManagerComponent::ItemInitializeBeforeSpawn(const EItemType ItemType, const FVector& Location)
{
	if (ItemClass == nullptr) return;

	FItemInfo NewItemInfo;

	// 아이템 타입 설정
	NewItemInfo.ItemType = ItemType;

	// 아이템 이름 설정
	const int32 RowNumber = StaticCast<int32>(ItemType);
	FItemOptionTableRow* ItemTableRow = ItemDataTable->FindRow<FItemOptionTableRow>(FName(*(FString::FormatAsNumber(RowNumber))), FString(""));
	NewItemInfo.ItemName = ItemTableRow->ItemName;

	// 아이템 스탯 설정
	if (ItemType == EItemType::EIT_Coin)
	{
		NewItemInfo.ItemStatArr.Add(FMath::RandRange(10, 100));
	}
	else if (ItemType == EItemType::EIT_HealthPotion || ItemType == EItemType::EIT_ManaPotion)
	{
		NewItemInfo.ItemStatArr.Add(300);
	}
	else if (ItemType == EItemType::EIT_Armour)
	{
		ArmourStatRandomInitialize(NewItemInfo);
	}
	else
	{
		AccessoriesStatRandomInitialize(NewItemInfo);
	}

	// 아이템 스폰
	ARPGItem* SpawnedItem = GetWorld()->SpawnActorDeferred<ARPGItem>(ItemClass, FTransform(FRotator::ZeroRotator, Location));
	if (SpawnedItem)
	{
		SpawnedItem->SetItemMesh(ItemTableRow->ItemMesh);
		SpawnedItem->SetItemInfo(NewItemInfo);
		SpawnedItem->FinishSpawning(FTransform(FRotator::ZeroRotator, Location));
	}
}

void UItemSpawnManagerComponent::ArmourStatRandomInitialize(FItemInfo& Info)
{
	// 스탯 옵션 개수
	const int8 MaxStatNum = 4;
	Info.ItemStatArr.Init(0, MaxStatNum);

	int32 StatBit = 0;

	// 첫번째 스탯
	RandomBitOn(StatBit, MaxStatNum);

	float ExtraStatPercentage = 60.f;
	if (FMath::RandRange(1, 100) < ExtraStatPercentage)
	{
		// 두번째 스탯
		RandomBitOn(StatBit, MaxStatNum);
		ExtraStatPercentage /= 4;
		if (FMath::RandRange(1, 100) < ExtraStatPercentage)
		{
			// 세번째 스탯
			RandomBitOn(StatBit, MaxStatNum);
			ExtraStatPercentage /= 4;
			if (FMath::RandRange(1, 100) < ExtraStatPercentage)
			{
				// 모든 스탯
				StatBit = (1 << MaxStatNum) - 1;
			}
		}
	}

	// DefenseivePower
	if (StatBit & (1 << 0))
	{
		Info.ItemStatArr[0] = (FMath::RandRange(1, 10) * 0.5f);
	}
	// Dexterity
	if (StatBit & (1 << 1))
	{
		Info.ItemStatArr[1] = (FMath::RandRange(10, 30) / 10.f);
	}
	// ExtraMP
	if (StatBit & (1 << 2))
	{
		Info.ItemStatArr[2] = (FMath::RandRange(5, 20) * 10);
	}
	// ExtraHP
	if (StatBit & (1 << 3))
	{
		Info.ItemStatArr[3] = (FMath::RandRange(10, 30) * 10);
	}
}

void UItemSpawnManagerComponent::RandomBitOn(int32& Bit, const int8 Range)
{
	if (Bit == ((1 << Range) - 1)) return;
	while (true)
	{
		int8 RandNum = FMath::RandRange(1, Range - 1);
		if (!(Bit & (1 << RandNum)))
		{
			Bit |= (1 << RandNum);
			return;
		}
	}
}

void UItemSpawnManagerComponent::AccessoriesStatRandomInitialize(FItemInfo& Info)
{
	// 스탯 옵션 개수
	const int8 MaxStatNum = 3;
	Info.ItemStatArr.Init(0, MaxStatNum);

	int32 StatBit = 0;

	// 첫번째 스탯
	RandomBitOn(StatBit, MaxStatNum);

	float ExtraStatPercentage = 50.f;
	if (FMath::RandRange(1, 100) < ExtraStatPercentage)
	{
		// 두번째 스탯
		RandomBitOn(StatBit, MaxStatNum);
		ExtraStatPercentage /= 10;
		if (FMath::RandRange(1, 100) < ExtraStatPercentage)
		{
			// 모든 스탯
			StatBit = (1 << MaxStatNum) - 1;
		}
	}

	// StrikingPower
	if (StatBit & (1 << 0))
	{
		Info.ItemStatArr[0] = (FMath::RandRange(1, 10) * 0.5f);
	}
	// SkillPower
	if (StatBit & (1 << 1))
	{
		Info.ItemStatArr[1] = (FMath::RandRange(1, 10) * 0.5f);
	}
	// AttackSpeed
	if (StatBit & (1 << 2))
	{
		Info.ItemStatArr[2] = (FMath::RandRange(10, 20) / 10.f);
	}
}


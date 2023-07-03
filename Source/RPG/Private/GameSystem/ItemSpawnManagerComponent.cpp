
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

void UItemSpawnManagerComponent::SpawnItems(const FVector& Location)
{
	int32 RandInt = FMath::RandRange(0, StaticCast<int32>(EItemType::EIT_MAX) - 1);
	const EItemType ItemType = StaticCast<EItemType>(RandInt);

	// 코인 무조건 스폰
	ItemInitializeBeforeSpawn(EItemType::EIT_Coin, GetRandomVector(Location));

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
			ItemInitializeBeforeSpawn(EItemType::EIT_Weapon, GetRandomVector(Location));
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
	switch (ItemType)
	{
	case EItemType::EIT_Coin:
		NewItemInfo.ItemStat.CoinAmount = FMath::RandRange(10, 100);
		break;
	case EItemType::EIT_HealthPotion:
	case EItemType::EIT_ManaPotion:
		NewItemInfo.ItemStat.RecoveryAmount = 300.f;
		break;
	case EItemType::EIT_Armour:
		ArmourStatRandomInitialize(NewItemInfo);
		break;
	case EItemType::EIT_Weapon:
		WeaponStatRandomInitialize(NewItemInfo);
		break;
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

	int32 StatBit = 0;

	// 첫번째 스탯
	const int8 FirstOption = FMath::RandRange(0, MaxStatNum - 1);
	StatBit &= (1 << FirstOption);

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

	FArmourStat ArmourStat;

	if (StatBit & (1 << 0))
	{
		ArmourStat.DefensivePower = FMath::RandRange(1, 10) * 0.5f;
	}
	if (StatBit & (1 << 1))
	{
		ArmourStat.Dexterity = FMath::RandRange(10, 30) / 10.f;
	}
	if (StatBit & (1 << 2))
	{
		ArmourStat.ExtraHP = FMath::RandRange(5, 20) * 10;
	}
	if (StatBit & (1 << 3))
	{
		ArmourStat.ExtraHP = FMath::RandRange(10, 30) * 10;
	}

	Info.ItemStat.ArmourStat = ArmourStat;
}

void UItemSpawnManagerComponent::RandomBitOn(int32& Bit, const int8 Range)
{
	if (Bit == ((1 << Range) - 1)) return;
	while (true)
	{
		int8 RandNum = FMath::RandRange(1, Range - 1);
		if (!(Bit & (1 << RandNum)))
		{
			Bit &= (1 << RandNum);
			return;
		}
	}
}

void UItemSpawnManagerComponent::WeaponStatRandomInitialize(FItemInfo& Info)
{
	// 스탯 옵션 개수
	const int8 MaxStatNum = 3;

	int32 StatBit = 0;

	// 첫번째 스탯
	const int8 FirstOption = FMath::RandRange(0, MaxStatNum - 1);
	StatBit &= (1 << FirstOption);

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

	FWeaponStat WeaponStat;

	if (StatBit & (1 << 0))
	{
		WeaponStat.StrikingPower = FMath::RandRange(1, 10) * 0.5f;
	}
	if (StatBit & (1 << 1))
	{
		WeaponStat.SkillPower = FMath::RandRange(1, 10) * 0.5f;
	}
	if (StatBit & (1 << 2))
	{
		WeaponStat.AttackSpeed = FMath::RandRange(10, 30) / 10.f;
	}

	Info.ItemStat.WeaponStat = WeaponStat;
}


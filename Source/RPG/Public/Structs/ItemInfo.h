#pragma once

#include "Enums/ItemType.h"
#include "ItemInfo.generated.h"

USTRUCT(Atomic)
struct FItemInfo
{
    GENERATED_BODY()
public:

    UPROPERTY()
    EItemType ItemType;

    UPROPERTY()
    FString ItemName;

    UPROPERTY()
    TArray<float> ItemStatArr;

    FItemInfo() : ItemType(EItemType::EIT_MAX), ItemName(FString(TEXT(""))) {}
    FItemInfo& operator=(const FItemInfo& Other)
    {
        if (this == &Other) return *this;

        ItemType = Other.ItemType;
        ItemName = Other.ItemName;
        ItemStatArr = Other.ItemStatArr;

        return *this;
    }
};
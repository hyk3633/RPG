
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/ItemType.h"
#include "Containers/Union.h"
#include "RPGItem.generated.h"

class UWidgetComponent;

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

UCLASS()
class RPG_API ARPGItem : public AActor
{
	GENERATED_BODY()
	
public:	

	ARPGItem();

	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

    void SetItemInfo(const FItemInfo& NewItemInfo);

    FORCEINLINE const FItemInfo GetItemInfo() const { return ItemInfo; }

    void SetItemMesh(UStaticMesh* NewMesh);

protected:

    UFUNCTION(NetMulticast, Reliable)
    void SetItemMeshMulticast(UStaticMesh* NewMesh);

public:

	void SetItemNameTagVisibility(const bool bVisible);

    void SetRenderCustomDepthOn(const int32 StencilValue);

    void SetRenderCustomDepthOff();

	void DestroyFromAllClients();

private:

	UFUNCTION(NetMulticast, Reliable)
	void DestroyMulticast();

    UFUNCTION()
    void OnRep_ItemInfo();

    void SetItemTagText();

protected:

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

private:

	UPROPERTY()
	USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* NameTagWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInfo)
	FItemInfo ItemInfo;
};

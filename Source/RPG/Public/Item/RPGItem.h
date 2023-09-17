
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/ItemType.h"
#include "Structs/ItemInfo.h"
#include "Containers/Union.h"
#include "RPGItem.generated.h"

DECLARE_MULTICAST_DELEGATE(FDelegateItemDeactivate)

class UWidgetComponent;
class UPaperSpriteComponent;

UCLASS()
class RPG_API ARPGItem : public AActor
{
	GENERATED_BODY()
	
public:	

	ARPGItem();

	virtual void PostInitializeComponents() override;

	FDelegateItemDeactivate DDeactivateItem;

protected:

	virtual void BeginPlay() override;

public:	

	void DeactivateItemFromAllClients();

	void ActivateItemFromAllClients(const FTransform& SpawnTransform);

protected:

	UFUNCTION(NetMulticast, Reliable)
	void DeactivateItemMulticast();

	void DeactivateItem();

	UFUNCTION(NetMulticast, Reliable)
	void ActivateItemMulticast(const FTransform& SpawnTransform);

	void ActivateItem(const FTransform& SpawnTransform);

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

private:

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

	UPROPERTY(EditAnywhere, Category = "Minimap", meta = (AllowPrivateAccess = "true"))
	UPaperSpriteComponent* ItemIconSprite;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* NameTagWidget;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInfo)
	FItemInfo ItemInfo;
};

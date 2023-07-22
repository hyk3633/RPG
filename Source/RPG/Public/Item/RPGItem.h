
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/ItemType.h"
#include "Structs/ItemInfo.h"
#include "Containers/Union.h"
#include "RPGItem.generated.h"

class UWidgetComponent;

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

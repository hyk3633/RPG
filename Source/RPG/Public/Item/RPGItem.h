
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enums/ItemType.h"
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

	void SetItemNameTagVisibility(const bool bVisible);

	FORCEINLINE EItemType GetItemType() const { return ItemType; }

	void DestroyFromAllClients();

private:

	UFUNCTION(NetMulticast, Reliable)
	void DestroyMulticast();

private:

	UPROPERTY()
	USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* NameTagWidget;

	UPROPERTY(EditAnywhere, Category = "Item Info")
	FString ItemName;

	float Amount = 100.f;

	UPROPERTY(EditAnywhere, Category = "Item Info")
	EItemType ItemType;
};

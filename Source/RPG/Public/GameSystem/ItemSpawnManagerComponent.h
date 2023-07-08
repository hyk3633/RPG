
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/ItemType.h"
#include "Engine/DataTable.h"
#include "ItemSpawnManagerComponent.generated.h"

class ARPGItem;
class UDataTable;
struct FItemInfo;

USTRUCT(BlueprintType)
struct FItemOptionTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UMaterial* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TArray<FString> PropertyNames;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_API UItemSpawnManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UItemSpawnManagerComponent();

protected:

	virtual void BeginPlay() override;

public:

	void DropItem(const FItemInfo& Info, const FVector& Location);

	void SpawnItems(const FVector& Location);

private:

	FVector& GetRandomVector(FVector Vector);

	void ItemInitializeBeforeSpawn(const EItemType ItemType, const FVector& Location);

	void ArmourStatRandomInitialize(FItemInfo& Info);

	void RandomBitOn(int32& Bit, const int8 Range);

	void AccessoriesStatRandomInitialize(FItemInfo& Info);

private:

	UPROPERTY(EditAnywhere, Category = "Item Spawner")
	TSubclassOf<ARPGItem> ItemClass;

	UPROPERTY(EditAnywhere, Category = "Item Spawner")
	UDataTable* ItemDataTable;

};

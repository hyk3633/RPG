#pragma once

#include "Engine/DataTable.h"
#include "ItemOptionTableRow.generated.h"

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
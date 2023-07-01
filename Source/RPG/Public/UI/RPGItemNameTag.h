
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGItemNameTag.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class RPG_API URPGItemNameTag : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetNameTagText(FString StrName);

private:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NameTag;
};

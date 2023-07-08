
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGStatTextBoxWidget.generated.h"

/**
 * 
 */

class UVerticalBox;
class UOverlay;
class UTextBlock;

UCLASS()
class RPG_API URPGStatTextBoxWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetStatText(const FString& StatString);

	void SetWidgetPosition(const FVector2D& Position);

private:

	UPROPERTY(meta = (BindWidget))
	UOverlay* StatOverlayBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* TextVerticalBox;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StatInfoText;
};

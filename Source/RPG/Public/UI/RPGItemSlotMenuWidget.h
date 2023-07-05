
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGItemSlotMenuWidget.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UScaleBox;

UCLASS()
class RPG_API URPGItemSlotMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetWidgetPosition(const FVector2D& Position);

	void SetUseText(FString ButtonText);

	FORCEINLINE UButton* GetUseButton() const { return UseButton; }
	FORCEINLINE UButton* GetDiscardButton() const { return DiscardButton; }

private:

	UPROPERTY(meta = (BindWidget))
	UScaleBox* WidgetBox;

	UPROPERTY(meta = (BindWidget))
	UButton* UseButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UseText;

	UPROPERTY(meta = (BindWidget))
	UButton* DiscardButton;

};

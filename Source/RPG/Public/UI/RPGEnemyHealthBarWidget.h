
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGEnemyHealthBarWidget.generated.h"

/**
 * 
 */

class UProgressBar;

UCLASS()
class RPG_API URPGEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* EnemyHealthProgressBar;

};

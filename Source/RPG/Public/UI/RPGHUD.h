
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RPGHUD.generated.h"

/**
 * 
 */

class URPGGameplayInterface;
class ARPGBasePlayerCharacter;

UCLASS()
class RPG_API ARPGHUD : public AHUD
{
	GENERATED_BODY()

public:

	ARPGHUD();

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;
	
protected:



	virtual void BeginPlay() override;

public:

	void InitHUD();

private:

	void DrawOverlay();

	/** ü��, ���� ���α׷��� �� */

	UFUNCTION()
	void SetHealthBarPercentage(float Percentage);

	UFUNCTION()
	void SetManaBarPercentage(float Percentage);

	/** ��ų ��ٿ� */

	void UpdateCooldownProgress();

	void SetProgressPercentage(const int8 Index, const float Percentage);

	UFUNCTION()
	void CooldownProgressSetFull(uint8 Bit);

	void OffHUD();

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UUserWidget> GameplayInterfaceClass;

	UPROPERTY()
	URPGGameplayInterface* GameplayInterface;

	UPROPERTY()
	ARPGBasePlayerCharacter* PlayerPawn;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstance* ClockProgressMatInst;

	UPROPERTY()
	UMaterialInstanceDynamic* ClockProgressMatInstDynamic;

	FTimerHandle OffTimer;

};
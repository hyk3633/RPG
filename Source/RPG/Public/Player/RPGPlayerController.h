
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGPlayerController.generated.h"

/**
 * 
 */

class UInputMappingContext;
class UInputAction;

UCLASS()
class RPG_API ARPGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ARPGPlayerController();

	virtual void PostInitializeComponents() override;

protected:

	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

public:

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void SetupInputComponent() override;

	void StopMove();

	void SetDestinationAndPath();

	void InitDestAndDir();

	UFUNCTION(Server, Reliable)
	void SetDestinaionAndPathServer(const FVector& HitLocation);

	void UpdateMovement();

	UFUNCTION()
	void OnRep_PathX();

	void NormalAttackPressed();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* NormalAttackClickAction;

	UPROPERTY(ReplicatedUsing = OnRep_PathX)
	TArray<float> PathX;

	UPROPERTY(Replicated)
	TArray<float> PathY;

	bool bUpdateMovement = false;
	
	FVector NextPoint;
	FVector NextDirection;
	int32 PathIdx;
};

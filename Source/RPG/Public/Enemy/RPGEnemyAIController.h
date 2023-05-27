
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RPGEnemyAIController.generated.h"

/**
 * 
 */

class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class RPG_API ARPGEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ARPGEnemyAIController();

protected:

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void FindClosestPlayer();

	void SetTarget(APawn* TargetToSet);

	UFUNCTION(BlueprintCallable)
	APawn* GetTarget() const;

private:

	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = "AI")
	UBlackboardData* BBAsset;

	UPROPERTY()
	UBehaviorTreeComponent* BTComp;

	UPROPERTY()
	UBlackboardComponent* BBComp;

};

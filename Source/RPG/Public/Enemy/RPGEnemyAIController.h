
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RPGEnemyAIController.generated.h"

/**
 * 
 */

class ARPGBaseEnemyCharacter;
class UBehaviorTreeComponent;
class UBlackboardComponent;

UCLASS()
class RPG_API ARPGEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ARPGEnemyAIController();

protected:

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void FindClosestPlayer();

	void SetTarget(APawn* TargetToSet);

	UFUNCTION(BlueprintCallable)
	APawn* GetTarget() const;

protected:

	UFUNCTION()
	void CharacterDead();

private:

	UPROPERTY()
	ARPGBaseEnemyCharacter* MyCharacter;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	UBehaviorTree* BTAsset;

	UPROPERTY(EditAnywhere, Category = "Enemy | AI")
	UBlackboardData* BBAsset;

	UPROPERTY()
	UBehaviorTreeComponent* BTComp;

	UPROPERTY()
	UBlackboardComponent* BBComp;

};

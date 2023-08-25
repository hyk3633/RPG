
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
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class RPG_API ARPGEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	ARPGEnemyAIController();

	static const FName IsDead;
	static const FName IsFalldown;
	static const FName IsRestrained;
	static const FName IsSuckedIn;
	static const FName TargetPlayer;
	static const FName IsStunned;

protected:

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:

	UFUNCTION(BlueprintCallable)
	APawn* GetTarget() const;

	void SetIsFalldown(const bool bIsFalldown);

	void SetIsRestrained(const bool bIsRestrained);

	void SetSuckedIn(const bool bIsSuckedIn);

	void SetIsStunned(const bool bIsStunned);

	bool GetSuckedIn() const;

protected:

	UFUNCTION()
	void CharacterDead();

	UFUNCTION()
	void CharacterActivate();

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

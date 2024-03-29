
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
	static const FName CanMeleeAttack;
	static const FName CanRangedAttack;
	static const FName CanSpecialAttack;

protected:

	virtual void PostInitializeComponents() override;

	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;

public:

	void BindFunctionToTargetDeadDelegate(ACharacter* NewTarget);

	UFUNCTION(BlueprintCallable)
	APawn* GetTarget() const;

	void EmptyTheTarget();

	void TargetMissed();

	void SetIsFalldown(const bool bIsFalldown);

	void SetIsRestrained(const bool bIsRestrained);

	void SetSuckedIn(const bool bIsSuckedIn);

	void SetIsStunned(const bool bIsStunned);

	void SetCanSpecialAttack(const bool bCanSpecialAttack);

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

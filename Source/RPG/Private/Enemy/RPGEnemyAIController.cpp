
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "GameSystem/EnemySpawner.h"
#include "../RPG.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

const FName ARPGEnemyAIController::IsDead(TEXT("IsDead"));
const FName ARPGEnemyAIController::IsFalldown(TEXT("IsFalldown"));
const FName ARPGEnemyAIController::IsRestrained(TEXT("IsRestrained"));
const FName ARPGEnemyAIController::IsSuckedIn(TEXT("IsSuckedIn"));
const FName ARPGEnemyAIController::TargetPlayer(TEXT("Target"));
const FName ARPGEnemyAIController::IsStunned(TEXT("IsStunned"));

ARPGEnemyAIController::ARPGEnemyAIController()
{

}

void ARPGEnemyAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyCharacter = Cast<ARPGBaseEnemyCharacter>(InPawn);
	if (MyCharacter)
	{
		MyCharacter->SetAIController(this);
		MyCharacter->DOnDeath.AddUFunction(this, FName("CharacterDead"));
		MyCharacter->DOnActivate.AddUFunction(this, FName("CharacterActivate"));
	}
	
	if (BBAsset)
	{
		if (!UseBlackboard(BBAsset, BBComp) || !InPawn) ELOG(TEXT("Failed Blackboard"));
	}

	if (BTAsset)
	{
		if (!RunBehaviorTree(BTAsset)) ELOG(TEXT("Failed Behaviortree"));
	}

	if (BBComp)
	{
		BBComp->SetValueAsBool(IsDead, true);
	}
}

void ARPGEnemyAIController::OnUnPossess()
{

}

APawn* ARPGEnemyAIController::GetTarget() const
{
	if (BBComp)
	{
		return Cast<APawn>(BBComp->GetValueAsObject(TargetPlayer));
	}
	return nullptr;
}

void ARPGEnemyAIController::SetIsFalldown(const bool bIsFalldown)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsFalldown, bIsFalldown);
	}
}

void ARPGEnemyAIController::SetIsRestrained(const bool bIsRestrained)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsRestrained, bIsRestrained);
	}
}

void ARPGEnemyAIController::SetSuckedIn(const bool bIsSuckedIn)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsSuckedIn, bIsSuckedIn);
	}
}

void ARPGEnemyAIController::SetIsStunned(const bool bIsStunned)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsStunned, bIsStunned);
	}
}

bool ARPGEnemyAIController::GetSuckedIn() const
{
	if (BBComp)
	{
		return BBComp->GetValueAsBool(IsSuckedIn);
	}
	else return false;
}

void ARPGEnemyAIController::CharacterDead()
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsDead, true);
	}
}

void ARPGEnemyAIController::CharacterActivate()
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(IsDead, false);
		BBComp->SetValueAsBool(IsRestrained, false);
		BBComp->SetValueAsBool(IsFalldown, false);
		BBComp->SetValueAsObject(TargetPlayer, nullptr);
	}
}


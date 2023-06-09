

#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPG.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

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
	}

	if (BBAsset)
	{
		if (!UseBlackboard(BBAsset, BBComp) || !InPawn) ELOG(TEXT("Failed Blackboard"));
	}

	if (BTAsset)
	{
		if (!RunBehaviorTree(BTAsset)) ELOG(TEXT("Failed Behaviortree"));
	}
}

void ARPGEnemyAIController::OnUnPossess()
{

}

void ARPGEnemyAIController::FindClosestPlayer()
{
	float MinDistance = 3000.f;
	ARPGBasePlayerCharacter* ClosestTarget = nullptr;

	APawn* MyPawn = GetPawn();
	if (MyPawn)
	{
		for (ARPGBasePlayerCharacter* Target : TActorRange<ARPGBasePlayerCharacter>(GetWorld()))
		{
			if (MyPawn->GetDistanceTo(Target) <= MinDistance)
			{
				MinDistance = MyPawn->GetDistanceTo(Target);
				ClosestTarget = Target;
			}
		}
	}

	if (ClosestTarget)
	{
		SetTarget(ClosestTarget);
	}
}

void ARPGEnemyAIController::SetTarget(APawn* TargetToSet)
{
	if (BBComp)
	{
		BBComp->SetValueAsObject(FName("Target"), TargetToSet);
		SetFocus(TargetToSet);
	}
}

APawn* ARPGEnemyAIController::GetTarget() const
{
	if (BBComp)
	{
		return Cast<APawn>(BBComp->GetValueAsObject(FName("Target")));
	}
	return nullptr;
}

void ARPGEnemyAIController::SetIsFalldown(const bool bIsFalldown)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(FName("IsFalldown"), bIsFalldown);
	}
}

void ARPGEnemyAIController::SetIsRestrained(const bool bIsRestrained)
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(FName("IsRestrained"), bIsRestrained);
	}
}

void ARPGEnemyAIController::CharacterDead()
{
	if (BBComp)
	{
		BBComp->SetValueAsBool(FName("IsDead"), true);
	}
}


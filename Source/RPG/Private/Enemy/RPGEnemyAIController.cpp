

#include "Enemy/RPGEnemyAIController.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPG.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"

ARPGEnemyAIController::ARPGEnemyAIController()
{
}

void ARPGEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BBAsset)
	{
		if (!UseBlackboard(BBAsset, BBComp) || !InPawn)
		{
			ELOG(TEXT("Failed Blackboard"));
		}
	}

	if (BTAsset)
	{
		if (!RunBehaviorTree(BTAsset))
		{
			ELOG(TEXT("Failed Behaviortree"));
		}
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

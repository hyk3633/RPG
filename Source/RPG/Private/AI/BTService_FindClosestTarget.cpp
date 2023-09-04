
#include "AI/BTService_FindClosestTarget.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Enemy/RPGEnemyFormComponent.h"
#include "GameSystem/EnemySpawner.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../RPG.h"

UBTService_FindClosestTarget::UBTService_FindClosestTarget()
{
	NodeName = TEXT("Find closest target");
	Interval = 5.f;
	bNotifyOnSearch = true;
}

void UBTService_FindClosestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Enemy == nullptr) return;

	ACharacter* ClosestTarget = Cast<ACharacter>(Enemy->GetTarget());
	TArray<ACharacter*>& Players = Enemy->GetSpawner()->GetPlayersInArea();
	if (Enemy->GetFormComponent()->GetEnemyType() == EEnemyType::EET_Boss)
	{
		const int8 PlayerCount = Players.Num();
		if (PlayerCount <= 0) return;

		const int8 NewTargetIdx = FMath::RandRange(0, PlayerCount - 1);
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(ARPGEnemyAIController::TargetPlayer, Players[NewTargetIdx]);
	}
	else
	{
		float MinDistance = Enemy->GetDetectDistance();
		for (ACharacter* Target : Players)
		{
			if (Enemy->GetDistanceTo(Target) <= MinDistance)
			{
				MinDistance = Enemy->GetDistanceTo(Target);
				ClosestTarget = Target;
			}
		}
		OwnerComp.GetBlackboardComponent()->SetValueAsObject(ARPGEnemyAIController::TargetPlayer, ClosestTarget);
	}
}

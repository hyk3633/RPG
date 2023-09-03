
#include "AI/BTTask_TurnToTarget.h"
#include "Enemy/RPGEnemyAIController.h"
#include "Enemy/Character/RPGBaseEnemyCharacter.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("Turn To Target");
}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ARPGBasePlayerCharacter* Player = Cast<ARPGBasePlayerCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ARPGEnemyAIController::TargetPlayer));
	if (Player == nullptr) return EBTNodeResult::Failed;

	ARPGBaseEnemyCharacter* Enemy = Cast<ARPGBaseEnemyCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if(Enemy == nullptr) return EBTNodeResult::Failed;

	FVector LookVector = Player->GetActorLocation() - Enemy->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();
	Enemy->SetActorRotation(FMath::RInterpTo(Enemy->GetActorRotation(), TargetRot, GetWorld()->GetDeltaSeconds(), 100.f));

	return EBTNodeResult::Succeeded;
}

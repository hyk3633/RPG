
#include "Player/RPGPlayerController.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Enums/PressedKey.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Net/UnrealNetwork.h"

ARPGPlayerController::ARPGPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

}

void ARPGPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ARPGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	/*int32 GridSize;
	int32 GridDist;
	float WorldOffset;

	GridSize = 80;
	GridDist = 25;
	WorldOffset = ((GridSize * GridDist) / 2.f) - (GridDist / 2.f);

	for (int i = 0; i < GridSize; i++)
	{
		for (int j = 0; j < GridSize; j++)
		{
			float X = (GridDist * j) - FMath::TruncToInt(WorldOffset);
			float Y = (GridDist * i) - FMath::TruncToInt(WorldOffset);

			DrawDebugPoint(GetWorld(), FVector(X, Y, 10.f), 10.f, FColor::Green, true);
		}
	}

	float X = GetPawn()->GetActorLocation().X;
	float Y = GetPawn()->GetActorLocation().Y;
	int32 j = FMath::Floor(((X + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	int32 i = FMath::Floor(((Y + FMath::TruncToInt(WorldOffset)) / GridDist) + 0.5f);
	DrawDebugPoint(GetWorld(), FVector((GridDist * j) - FMath::TruncToInt(WorldOffset), (GridDist * i) - FMath::TruncToInt(WorldOffset), 10.f), 10.f, FColor::Red, true);*/
}

void ARPGPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyCharacter = Cast<ARPGBasePlayerCharacter>(InPawn);
	if (MyCharacter == nullptr)
		ELOG(TEXT("Player Pawn Cast Failed!"));
}

void ARPGPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ARPGPlayerController::SetDestinationClick_StopMove);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::SetDestinationClick_SetPath);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::RightClick_AttackOrSetAbilityPoint);
		EnhancedInputComponent->BindAction(Ability_Q_PressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::Ability_Q_PressedAction_Cast);
		EnhancedInputComponent->BindAction(Ability_W_PressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::Ability_W_PressedAction_Cast);
		EnhancedInputComponent->BindAction(Ability_E_PressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::Ability_E_PressedAction_Cast);
		EnhancedInputComponent->BindAction(Ability_R_PressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::Ability_R_PressedAction_Cast);
	}
}

void ARPGPlayerController::SetDestinationClick_StopMove()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->StopMove();
}

void ARPGPlayerController::SetDestinationClick_SetPath()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->SetDestinationAndPath();
}

void ARPGPlayerController::RightClick_AttackOrSetAbilityPoint()
{
	if (MyCharacter == nullptr) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CastAbilityAfterTargeting();
	}
	else
	{
		MyCharacter->DoNormalAttack();
	}
}

void ARPGPlayerController::Ability_Q_PressedAction_Cast()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->CastAbilityByKey(EPressedKey::EPK_Q);
}

void ARPGPlayerController::Ability_W_PressedAction_Cast()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->CastAbilityByKey(EPressedKey::EPK_W);
}

void ARPGPlayerController::Ability_E_PressedAction_Cast()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->CastAbilityByKey(EPressedKey::EPK_E);
}

void ARPGPlayerController::Ability_R_PressedAction_Cast()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->CastAbilityByKey(EPressedKey::EPK_R);
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, MyCharacter);
}


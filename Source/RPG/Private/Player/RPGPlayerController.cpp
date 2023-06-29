
#include "Player/RPGPlayerController.h"
#include "Player/Character/RPGBasePlayerCharacter.h"
#include "UI/RPGHUD.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
#include "Enums/PressedKey.h"
#include "GameFramework/HUD.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"

ARPGPlayerController::ARPGPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> Obj_DefaultContext (TEXT("/Game/_Assets/Input/IMC_DefaulnputMappingContext.IMC_DefaulnputMappingContext"));
	if (Obj_DefaultContext.Succeeded()) DefaultMappingContext = Obj_DefaultContext.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_LeftClick (TEXT("/Game/_Assets/Input/IA_LeftClick.IA_LeftClick"));
	if (Obj_LeftClick.Succeeded()) LeftClickAction = Obj_LeftClick.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_RightClick (TEXT("/Game/_Assets/Input/IA_RightClick.IA_RightClick"));
	if (Obj_RightClick.Succeeded()) RightClickAction = Obj_RightClick.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_Q (TEXT("/Game/_Assets/Input/IA_Q.IA_Q"));
	if (Obj_Q.Succeeded()) QPressedAction = Obj_Q.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_W (TEXT("/Game/_Assets/Input/IA_W.IA_W"));
	if (Obj_W.Succeeded()) WPressedAction = Obj_W.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_E (TEXT("/Game/_Assets/Input/IA_E.IA_E"));
	if (Obj_E.Succeeded()) EPressedAction = Obj_E.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_R (TEXT("/Game/_Assets/Input/IA_R.IA_R"));
	if (Obj_R.Succeeded()) RPressedAction = Obj_R.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> Obj_MouseWheelScroll (TEXT("/Game/_Assets/Input/IA_ZoomInOut.IA_ZoomInOut"));
	if (Obj_MouseWheelScroll.Succeeded()) MouseWheelScroll = Obj_MouseWheelScroll.Object;
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
}

void ARPGPlayerController::OnRep_MyCharacter()
{
	ARPGHUD* RPGHUD = Cast<ARPGHUD>(GetHUD());
	if (RPGHUD) RPGHUD->InitHUD();
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
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Started, this, &ARPGPlayerController::LeftClickAction_StopMove);
		EnhancedInputComponent->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::LeftClickAction_SetPath);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::RightClick_AttackOrSetAbilityPoint);
		EnhancedInputComponent->BindAction(QPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::QPressedAction_Cast);
		EnhancedInputComponent->BindAction(WPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::WPressedAction_Cast);
		EnhancedInputComponent->BindAction(EPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::EPressedAction_Cast);
		EnhancedInputComponent->BindAction(RPressedAction, ETriggerEvent::Completed, this, &ARPGPlayerController::RPressedAction_Cast);
		EnhancedInputComponent->BindAction(MouseWheelScroll, ETriggerEvent::Triggered, this, &ARPGPlayerController::MouseWheelScroll_ZoomInOut);
	}
}

void ARPGPlayerController::LeftClickAction_StopMove()
{
	if (MyCharacter == nullptr) return;
	MyCharacter->StopMove();
}

void ARPGPlayerController::LeftClickAction_SetPath()
{
	if (MyCharacter == nullptr) return;
	if (MyCharacter->GetAbilityERMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else
	{
		MyCharacter->SetDestinationAndPath();
	}
}

void ARPGPlayerController::RightClick_AttackOrSetAbilityPoint()
{
	if (MyCharacter == nullptr && MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->GetCursorHitResultCastAbility();
	}
	else
	{
		MyCharacter->DoNormalAttack();
	}
}

void ARPGPlayerController::QPressedAction_Cast()
{
	if (MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if(MyCharacter->IsAbilityAvailable(EPressedKey::EPK_Q))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_Q);
	}
}

void ARPGPlayerController::WPressedAction_Cast()
{
	if (MyCharacter == nullptr && MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_W))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_W);
	}
}

void ARPGPlayerController::EPressedAction_Cast()
{
	if (MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_E))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_E);
	}
}

void ARPGPlayerController::RPressedAction_Cast()
{
	if (MyCharacter == nullptr || MyCharacter->GetIsAnyMontagePlaying()) return;

	if (MyCharacter->GetAiming())
	{
		MyCharacter->CancelAbility();
	}
	else  if (MyCharacter->IsAbilityAvailable(EPressedKey::EPK_R))
	{
		MyCharacter->ReadyToCastAbilityByKey(EPressedKey::EPK_R);
	}
}

void ARPGPlayerController::MouseWheelScroll_ZoomInOut(const FInputActionValue& Value)
{
	if (MyCharacter == nullptr) return;

	MyCharacter->CameraZoomInOut(-Value.Get<float>());
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, MyCharacter);
}


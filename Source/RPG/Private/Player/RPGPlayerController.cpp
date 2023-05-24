
#include "Player/RPGPlayerController.h"
#include "../RPGGameModeBase.h"
#include "../RPG.h"
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

}

void ARPGPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bUpdateMovement)
	{
		UpdateMovement();
	}
}

void ARPGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ARPGPlayerController::StopMove);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ARPGPlayerController::SetDestinationAndPath);
	}
}

void ARPGPlayerController::StopMove()
{
	StopMovement();
	bUpdateMovement = false;
	PathIdx = 0;
}

void ARPGPlayerController::SetDestinationAndPath()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (Hit.bBlockingHit == false)
	{
		WLOG(TEXT("Nothing Hit"));
		return;
	}

	//SpawnClickParticle(Hit.ImpactPoint);

	if (HasAuthority())
	{
		GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetPawn()->GetActorLocation(), Hit.ImpactPoint, PathX, PathY);
		InitDestAndDir();
	}
	else
	{
		SetDestinaionAndPathServer(Hit.ImpactPoint);
	}

	/*if (HasAuthority())
	{
		for (int32 i = 0; i < PathX.Num(); i++)
		{
			DrawDebugPoint(GetWorld(), FVector(PathX[i], PathY[i], 10.f), 10.f, FColor::Blue, false, 2.f);
		}
	}*/
}

void ARPGPlayerController::InitDestAndDir()
{
	bUpdateMovement = true;
	NextPoint = FVector(PathX[0], PathY[0], GetPawn()->GetActorLocation().Z);
	NextDirection = (NextPoint - GetPawn()->GetActorLocation()).GetSafeNormal();
}

void ARPGPlayerController::SetDestinaionAndPathServer_Implementation(const FVector& HitLocation)
{
	GetWorld()->GetAuthGameMode<ARPGGameModeBase>()->GetPathToDestination(GetPawn()->GetActorLocation(), HitLocation, PathX, PathY);
}

void ARPGPlayerController::UpdateMovement()
{
	if (FVector::Dist(NextPoint, GetPawn()->GetActorLocation()) > 20.f)
	{
		GetPawn()->AddMovementInput(NextDirection);
	}
	else
	{
		PathIdx++;
		if (PathIdx == PathX.Num())
		{
			bUpdateMovement = false;
			PathIdx = 0;
		}
		else
		{
			NextPoint = FVector(PathX[PathIdx], PathY[PathIdx], GetPawn()->GetActorLocation().Z);
			NextDirection = (NextPoint - GetPawn()->GetActorLocation()).GetSafeNormal();
		}
	}
}

void ARPGPlayerController::OnRep_PathX()
{
	InitDestAndDir();
}

void ARPGPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGPlayerController, PathX);
	DOREPLIFETIME(ARPGPlayerController, PathY);
}


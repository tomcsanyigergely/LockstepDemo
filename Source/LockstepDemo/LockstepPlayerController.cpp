// Fill out your copyright notice in the Description page of Project Settings.


#include "LockstepPlayerController.h"

#include "CollisionChannels.h"
#include "DrawDebugHelpers.h"
#include "LockstepGameModeBase.h"
#include "LockstepUnitActor.h"
#include "GameFramework/PlayerState.h"


void ALockstepPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &ALockstepPlayerController::OnLeftMouseButtonPressed);
	InputComponent->BindAction("LeftMouseButton", IE_Released, this, &ALockstepPlayerController::OnLeftMouseButtonReleased);
	InputComponent->BindAction("RightMouseButton", IE_Pressed, this, &ALockstepPlayerController::OnRightMouseButtonPressed);
}

void ALockstepPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);
}

void ALockstepPlayerController::OnLeftMouseButtonPressed()
{
	bIsSelecting = true;
	GetMousePosition(MouseSelectStartScreenPosition.X, MouseSelectStartScreenPosition.Y);
}

void ALockstepPlayerController::OnLeftMouseButtonReleased()
{
	FVector2D MouseSelectEndScreenPosition;	
	GetMousePosition(MouseSelectEndScreenPosition.X, MouseSelectEndScreenPosition.Y);

	FVector MouseSelectStartWorldLocation, MouseSelectStartWorldDirection;
	FVector MouseSelectEndWorldLocation, MouseSelectEndWorldDirection;

	DeprojectScreenPositionToWorld(MouseSelectStartScreenPosition.X, MouseSelectStartScreenPosition.Y, MouseSelectStartWorldLocation, MouseSelectStartWorldDirection);
	DeprojectScreenPositionToWorld(MouseSelectEndScreenPosition.X, MouseSelectEndScreenPosition.Y, MouseSelectEndWorldLocation, MouseSelectEndWorldDirection);

	FVector OverlapPos = (MouseSelectStartWorldLocation + MouseSelectEndWorldLocation) / 2;
	OverlapPos.Z = 0;

	FVector OverlapBoxHalfExtents(FMath::Abs(MouseSelectEndWorldLocation.X-MouseSelectStartWorldLocation.X)/2, FMath::Abs(MouseSelectEndWorldLocation.Y-MouseSelectStartWorldLocation.Y)/2, 10);
	
	TArray<FOverlapResult> Overlaps;
	
	GetWorld()->OverlapMultiByChannel(Overlaps, OverlapPos, FQuat::Identity, ECC_GameTraceChannel_SelectUnit, FCollisionShape::MakeBox(OverlapBoxHalfExtents));

	SelectedUnits.Empty();

	for(const FOverlapResult& OverlapResult : Overlaps)
	{
		SelectedUnits.Add(Cast<ALockstepUnitActor>(OverlapResult.Actor)->UnitID);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Selected %d units"), Overlaps.Num())

	bIsSelecting = false;
}

void ALockstepPlayerController::OnRightMouseButtonPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("RightClick"))

	if (SelectedUnits.Num() > 0)
	{
		FHitResult HitResult;
		if (GetHitResultUnderCursor(ECC_GameTraceChannel_DetectGround, false, HitResult))
		{
			DrawDebugSphere(GetWorld(), HitResult.Location, 20, 20, FColor::Red, false, 0.5f);
			MoveCommand(fixed(HitResult.Location.X).raw_value(), fixed(HitResult.Location.Y).raw_value(), SelectedUnits);
		}
	}
}

void ALockstepPlayerController::MoveCommand_Implementation(int X, int Y, const TArray<uint16>& Units)
{
	if (Units.Num() > 0)
	{
		FLockstepMoveCommand MoveCommand;
		MoveCommand.PlayerId = PlayerState->GetPlayerId() - 256;
		MoveCommand.Units = Units;
		MoveCommand.X = X;
		MoveCommand.Y = Y;

		GetWorld()->GetAuthGameMode<ALockstepGameModeBase>()->AddMoveCommand(MoveCommand);
	}
}

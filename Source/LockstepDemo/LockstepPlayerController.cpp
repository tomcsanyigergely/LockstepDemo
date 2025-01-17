// Fill out your copyright notice in the Description page of Project Settings.


#include "LockstepPlayerController.h"

#include "DrawDebugHelpers.h"
#include "LockstepGameModeBase.h"
#include "GameFramework/PlayerState.h"


void ALockstepPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("RightClick", IE_Pressed, this, &ALockstepPlayerController::OnRightMouseButtonPressed);
}

void ALockstepPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(true);
}

void ALockstepPlayerController::OnRightMouseButtonPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("RightClick"))

	FHitResult HitResult;
	if (GetHitResultUnderCursor(ECC_WorldStatic, false, HitResult))
	{
		DrawDebugSphere(GetWorld(), HitResult.Location, 20, 20, FColor::Red, false, 0.5f);
		MoveCommand(fixed(HitResult.Location.X).raw_value(), fixed(HitResult.Location.Y).raw_value());
	}
}

void ALockstepPlayerController::MoveCommand_Implementation(int X, int Y)
{
	FLockstepMoveCommand MoveCommand;
	MoveCommand.PlayerId = PlayerState->GetPlayerId() - 256;
	MoveCommand.X = X;
	MoveCommand.Y = Y;

	GetWorld()->GetAuthGameMode<ALockstepGameModeBase>()->AddMoveCommand(MoveCommand);
}

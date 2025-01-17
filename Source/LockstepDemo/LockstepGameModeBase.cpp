// Copyright Epic Games, Inc. All Rights Reserved.


#include "LockstepGameModeBase.h"

ALockstepGameModeBase::ALockstepGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALockstepGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Tick: %d, PendingMoveCommands: %d"), TickIndex, PendingMoveCommands.Num())

	GetGameState<ALockstepGameStateBase>()->LockstepTick(TickIndex, PendingMoveCommands);

	PendingMoveCommands.Empty();
	TickIndex++;
}

void ALockstepGameModeBase::AddMoveCommand(FLockstepMoveCommand Command)
{
	PendingMoveCommands.Add(Command);
}

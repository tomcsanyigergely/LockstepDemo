// Fill out your copyright notice in the Description page of Project Settings.


#include "LockstepGameStateBase.h"

void ALockstepGameStateBase::LockstepTick_Implementation(uint16 TickIndex, const TArray<FLockstepMoveCommand>& MoveCommands)
{
	if (!GetWorld()->IsServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("Received Tick: %d"), TickIndex)

		for (const FLockstepMoveCommand& MoveCommand : MoveCommands) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Received MoveCommand: %d %f %f"), MoveCommand.PlayerId, MoveCommand.X, MoveCommand.Y)
		}

		SafeInitializeSimulation();

		for(auto Command : MoveCommands)
		{
			Simulation.ProcessMoveCommand(Command.X, Command.Y);
		}

		Simulation.Tick();

		if (BeginPlayCalled)
		{
			Render();
		}
	}
}

void ALockstepGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	SafeInitializeSimulation();

	if (GetWorld()->IsClient())
	{
		for(int i = 0; i < Simulation.GetUnitCount(); i++)
		{
			FVector2D UnitLocation = Simulation.GetUnitPosition(i);
			FVector SpawnLocation = FVector(UnitLocation.X, UnitLocation.Y, 0);
			FRotator SpawnRotation = FRotator::ZeroRotator;
			AActor* UnitActor = GetWorld()->SpawnActor(UnitActorClass, &SpawnLocation, &SpawnRotation);
			UnitActors.Add(UnitActor);
		}
	}

	BeginPlayCalled = true;
}

void ALockstepGameStateBase::Render()
{
	for(int i = 0; i < Simulation.GetUnitCount(); i++)
	{
		FVector2D UnitLocation = Simulation.GetUnitPosition(i);
		UnitActors[i]->SetActorLocation(FVector(UnitLocation.X, UnitLocation.Y, 0));
	}
}

void ALockstepGameStateBase::SafeInitializeSimulation()
{
	if (!SimulationInitialized)
	{
		Simulation.Initialize();
		SimulationInitialized = true;
	}
}

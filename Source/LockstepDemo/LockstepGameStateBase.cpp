// Fill out your copyright notice in the Description page of Project Settings.

#include "LockstepGameStateBase.h"

#include "LockstepUnitActor.h"

void ALockstepGameStateBase::LockstepTick_Implementation(uint16 TickIndex, const TArray<FLockstepMoveCommand>& MoveCommands)
{
	if (!GetWorld()->IsServer())
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Received Tick: %d"), TickIndex)

		for (const FLockstepMoveCommand& MoveCommand : MoveCommands) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Received MoveCommand: %d %f %f"), MoveCommand.PlayerId, MoveCommand.X, MoveCommand.Y)
		}*/

		SafeInitializeSimulation();

		for(auto MoveCommand : MoveCommands)
		{
			Simulation.ProcessMoveCommand(MoveCommand);
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
			ALockstepUnitActor* UnitActor = GetWorld()->SpawnActor<ALockstepUnitActor>(UnitActorClass, FVector(UnitLocation.X, UnitLocation.Y, 0), FRotator::ZeroRotator);
			UnitActor->UnitID = i;
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

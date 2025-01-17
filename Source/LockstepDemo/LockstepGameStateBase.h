// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LockstepSimulation.h"
#include "GameFramework/GameStateBase.h"
#include "LockstepGameStateBase.generated.h"

USTRUCT()
struct FLockstepMoveCommand
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 PlayerId;
	
	UPROPERTY()
	int X;
	
	UPROPERTY()
	int Y;
};

/**
 * 
 */
UCLASS()
class LOCKSTEPDEMO_API ALockstepGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	FLockstepSimulation Simulation;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> UnitActorClass;

	UPROPERTY()
	TArray<AActor*> UnitActors;

    bool SimulationInitialized = false;
	bool BeginPlayCalled = false;

public:
	virtual void BeginPlay() override;	
	
	UFUNCTION(NetMulticast, Reliable)
	void LockstepTick(uint16 TickIndex, const TArray<FLockstepMoveCommand>& MoveCommands);

	void Render();

private:
	void SafeInitializeSimulation();
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LockstepGameStateBase.h"
#include "GameFramework/GameModeBase.h"
#include "LockstepGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class LOCKSTEPDEMO_API ALockstepGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	TArray<FLockstepMoveCommand> PendingMoveCommands;

	uint16 TickIndex = 0;

public:
	ALockstepGameModeBase(const FObjectInitializer& ObjectInitializer);
	
	virtual void Tick(float DeltaSeconds) override;

	void AddMoveCommand(FLockstepMoveCommand Command);
};

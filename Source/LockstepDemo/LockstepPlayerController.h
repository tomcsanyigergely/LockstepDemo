// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LockstepPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOCKSTEPDEMO_API ALockstepPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRightMouseButtonPressed();

	UFUNCTION(Server, Reliable)
	void MoveCommand(int X, int Y);
};

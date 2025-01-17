// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LockstepUnitActor.generated.h"

UCLASS()
class LOCKSTEPDEMO_API ALockstepUnitActor : public AActor
{
	GENERATED_BODY()

public:
	int UnitID;
	
public:	
	// Sets default values for this actor's properties
	ALockstepUnitActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

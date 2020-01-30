// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Engine/World.h"

AMyAIController::AMyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UWorld* world = GetWorld())
		MoveToActor(world->GetFirstPlayerController()->GetPawn(), AcceptanceRadius);
}

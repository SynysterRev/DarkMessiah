// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class DARKMESSIAH_API AMyAIController : public AAIController
{
	GENERATED_BODY()
		AMyAIController();
protected:
	UPROPERTY(EditAnywhere)
		float AcceptanceRadius;
private :
	void Tick(float DeltaTime) override;
};

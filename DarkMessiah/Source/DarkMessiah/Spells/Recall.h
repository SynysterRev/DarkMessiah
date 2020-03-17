// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Spell.h"
#include "Recall.generated.h"

/**
 *
 */
UCLASS()
class DARKMESSIAH_API ARecall : public ASpell
{
	GENERATED_BODY()
public :
	void LaunchSpell(FVector _direction) override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Executing Recall", Category = "LaunchRecall")
	void Event_RecallLaunch_BP();

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "Ending Recall", Category = "LaunchRecall")
	void Event_RecallEnd_BP();
protected:

	UPROPERTY(EditAnywhere)
	int32 MaxPositionRemembered;

	UPROPERTY(EditAnywhere)
	float TimeBetweenEachPosition;
	
	UPROPERTY()
	TArray<FTransform> PreviousPositions;

	void Tick(float _deltaTime) override;
	void BeginPlay() override;

private:
	FTimerHandle TimerHandlePosition;
	void RegisterPosition();
	UWorld* world;
	FVector nextPosition;
	FVector LastPosition;
	bool IsSpellCast;
	float GravityScale;
	float Timer = 0.0f;
};

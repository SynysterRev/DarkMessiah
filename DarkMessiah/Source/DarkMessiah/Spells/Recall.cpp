// Fill out your copyright notice in the Description page of Project Settings.


#include "Recall.h"
#include "DarkMessiahCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Helpers/HelperLibrary.h"

void ARecall::LaunchSpell(FVector _direction)
{
	IsSpellCast = true;
	if (world != nullptr)
	{
		world->GetTimerManager().ClearTimer(TimerHandlePosition);
	}
	if (Caster != nullptr)
	{
		if (APlayerController* pController = Cast<APlayerController>(Caster->GetController()))
			Caster->DisableInput(pController);

		if (Caster != nullptr)
		{
			GravityScale = Caster->GetCharacterMovement()->GravityScale;
			Caster->GetCharacterMovement()->GravityScale = 0.0f;
		}
	}

}

void ARecall::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
	if (IsSpellCast)
	{
		if (PreviousPositions.Num() == 0)
		{
			IsSpellCast = false;
			if (world != nullptr)
			{
				world->GetTimerManager().SetTimer(TimerHandlePosition, this, &ARecall::RegisterPosition, TimeBetweenEachPosition, true);
			}
			if (Caster != nullptr)
			{
				if (APlayerController* pController = Cast<APlayerController>(Caster->GetController()))
					Caster->EnableInput(pController);
				Caster->GetCharacterMovement()->GravityScale = GravityScale;
			}
		}
		else
		{
			FTransform tr = UKismetMathLibrary::TLerp(Caster->GetTransform(), PreviousPositions[PreviousPositions.Num() - 1], _deltaTime * 80.0f);
			Caster->SetActorLocation(tr.GetLocation());
			FRotator rotator = UKismetMathLibrary::RLerp(Caster->GetActorRotation(), tr.GetRotation().Rotator(), _deltaTime * 50.0f, false);
			Caster->SetActorRotation(rotator);
			if (UKismetMathLibrary::NearlyEqual_TransformTransform(Caster->GetTransform(), PreviousPositions[PreviousPositions.Num() - 1], 5.0f, 5.0f))
			{
				PreviousPositions.RemoveAt(PreviousPositions.Num() - 1);
			}
		}
	}
}

void ARecall::BeginPlay()
{
	Super::BeginPlay();
	world = GetWorld();
	if (world != nullptr)
	{
		world->GetTimerManager().SetTimer(TimerHandlePosition, this, &ARecall::RegisterPosition, TimeBetweenEachPosition, true);
	}
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ARecall::RegisterPosition()
{
	if (Caster != nullptr)
	{
		PreviousPositions.Add(Caster->GetTransform());
		FVector extends(5.0f, 5.0f, 5.0f);
		DrawDebugBox(world, Caster->GetActorLocation(), extends, FColor::Yellow, false, MaxPositionRemembered * TimeBetweenEachPosition + 1.0f, 0, 10.0f);
		if (PreviousPositions.Num() > MaxPositionRemembered)
			PreviousPositions.RemoveAt(0);
	}
}

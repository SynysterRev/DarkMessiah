// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBall.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/Engine.h"
#include "DarkMessiahCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Helpers/HelperLibrary.h"


AFireBall::AFireBall()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	CurrentScale = GetActorScale3D();
	CurrentLocation = GetActorLocation();
	PowerMultiplicator = 1.0f;
	CurrentPowerMultiplicator = PowerMultiplicator;
	RootComponent = CollisionComp;
}

void AFireBall::IncreasePower()
{
	if (CurrentScale.X < MaxScale.X && CurrentScale.Y < MaxScale.Y && CurrentScale.Z < MaxScale.Z)
	{
		CurrentScale += AddingScale;
		CurrentLocation += AddingScale * 4.0f;
		SetActorScale3D(CurrentScale);
		CurrentPowerMultiplicator = FMath::Clamp(PowerMultiplicator + CurrentScale.X / MaxScale.X, PowerMultiplicator, MaxPowerMultiplicator);

		SetActorRelativeLocation(CurrentLocation);
	}
	else
	{
		if (UWorld* world = GetWorld())
		{
			if (world->GetTimerManager().IsTimerActive(TimerPreparation))
			{
				world->GetTimerManager().ClearTimer(TimerPreparation);
			}
		}
	}
}

void AFireBall::LaunchSpell(FVector _direction)
{
	if (UWorld* world = GetWorld())
	{
		if (world->GetTimerManager().IsTimerActive(TimerPreparation))
		{
			world->GetTimerManager().ClearTimer(TimerPreparation);
		}
	}
	Strength *= CurrentPowerMultiplicator;
	GetProjectileMovement()->Velocity = _direction * Speed;
	SetLifeSpan(2.0f);
	GetSphereComponent()->SetCollisionProfileName(TEXT("Projectile"));
}

void AFireBall::PrepareSpell()
{
	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().SetTimer(TimerPreparation, this, &AFireBall::IncreasePower, TimerIncrease, true);
	}
}


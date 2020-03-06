// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Helpers/HelperLibrary.h"
#include "CharacterAI.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ABlackHole::ABlackHole()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	TriggerComp = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComp"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = CollisionComp;
	TriggerComp->SetupAttachment(CollisionComp);
	MeshComponent->SetupAttachment(CollisionComp);
	PrimaryActorTick.bCanEverTick = true;
}

void ABlackHole::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherComp->Mobility == EComponentMobility::Static)
		{
			if (ProjectileMovement != nullptr)
				ProjectileMovement->Velocity = FVector::ZeroVector;
		}
	}
}

void ABlackHole::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OverlappedComponent != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (ACharacterAI* charact = Cast<ACharacterAI>(OtherActor))
		{
			EnemiesOverlapped.Add(charact);
		}
	}
}

void ABlackHole::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OverlappedComponent != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (ACharacterAI* charact = Cast<ACharacterAI>(OtherActor))
		{
			if (EnemiesOverlapped.Contains(charact))
			{
				EnemiesOverlapped.Remove(charact);
			}
		}
	}
}

void ABlackHole::LaunchSpell(FVector _direction)
{
	IsLaunch = true;
	LastPosition = GetActorLocation();
	if (ProjectileMovement != nullptr)
		ProjectileMovement->Velocity = _direction * Speed;
	CollisionComp->SetCollisionProfileName(TEXT("ProjectileWithoutColl"));
	TriggerComp->SetCollisionProfileName(TEXT("TriggerDmg"));
	if (UWorld* world = GetWorld())
	{
		FTimerHandle TimerHandleDamage;
		world->GetTimerManager().SetTimer(TimerHandleDamage, this, &ABlackHole::InflictDamage, TimerDamage, true);
	}
}

void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddDynamic(this, &ABlackHole::OnHit);
	TriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::BeginOverlap);
	TriggerComp->OnComponentEndOverlap.AddDynamic(this, &ABlackHole::EndOverlap);
	DistorsionActor = FindComponentByClass<UActorComponent>();
}

void ABlackHole::Tick(float _deltaTime)
{
	Super::Tick(_deltaTime);
	if (IsLaunch)
	{
		CurrentDistance += FVector::Dist(GetActorLocation(), LastPosition);
		LastPosition = GetActorLocation();
		if (CurrentDistance >= MaxDistance)
		{
			if (ProjectileMovement != nullptr)
				ProjectileMovement->Velocity = FVector::ZeroVector;
			IsLaunch = false;
			SetLifeSpan(SpanLife);
		}
	}
}

void ABlackHole::InflictDamage()
{
	for (int i = 0; i < EnemiesOverlapped.Num(); ++i)
	{
		if (EnemiesOverlapped[i] != nullptr)
		{
			EnemiesOverlapped[i]->TakeDamage(Damage, this);
		}
	}
}

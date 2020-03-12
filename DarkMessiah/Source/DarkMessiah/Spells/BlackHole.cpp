// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/ActorComponent.h"
#include "Helpers/HelperLibrary.h"
#include "CharacterAI.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

ABlackHole::ABlackHole()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	TriggerComp = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComp"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	WaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaveMesh"));
	DistorsionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DistorsionMesh"));
	SecondRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SecondRoot"));
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;
	CollisionComp->SetupAttachment(RootComp);
	TriggerComp->SetupAttachment(RootComp);
	SecondRootComponent->SetupAttachment(CollisionComp);

	MeshComponent->SetupAttachment(SecondRootComponent);
	WaveMesh->SetupAttachment(SecondRootComponent);
	DistorsionMesh->SetupAttachment(SecondRootComponent);
	PrimaryActorTick.bCanEverTick = true;
}

void ABlackHole::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((HitComp != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherComp->Mobility == EComponentMobility::Static)
		{
			if (ProjectileMovement != nullptr)
				ProjectileMovement->Velocity = FVector::ZeroVector;
			IsLaunch = false;
			SetLifeSpan(SpanLife);
		}
	}
}

void ABlackHole::BeginOverlapHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OverlappedComponent != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		if (OtherComp->Mobility == EComponentMobility::Static)
		{
			if (ProjectileMovement != nullptr)
				ProjectileMovement->Velocity = FVector::ZeroVector;
			IsLaunch = false;
			SetLifeSpan(SpanLife);
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
			charact->SlowCharacter(PercentageSlow / 100.0f);
			if (USkeletalMeshComponent* skeletalMesh = charact->GetMesh())
			{				
				if (skeletalMesh->GetMaterial(0) != nullptr)
				{
					UPhysicalMaterial* physMat = skeletalMesh->GetMaterial(0)->GetPhysicalMaterial();
					if (physMat != nullptr && physMat->SurfaceType == SurfaceType1)
					{
						EnemiesMeshesOverlapped.Add(skeletalMesh);
						TimersAbsorption.Add(1.0f);
					}
				}
			}
		}
	}
}

void ABlackHole::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OverlappedComponent != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && CanGrow)
	{
		if (ACharacterAI* charact = Cast<ACharacterAI>(OtherActor))
		{
			if (EnemiesOverlapped.Contains(charact))
			{
				EnemiesOverlapped.Remove(charact);
				float verif = (PercentageSlow / 100.0f);
				if (verif > 0.0f)
					charact->SpeedUpCharacter(1.0f / verif);
			}
			if (USkeletalMeshComponent* skeletalMesh = charact->GetMesh())
			{
				if (EnemiesMeshesOverlapped.Contains(skeletalMesh))
				{
					int32 indexEnemy = EnemiesMeshesOverlapped.Find(skeletalMesh);
					TimersAbsorption.RemoveAt(indexEnemy);
					EnemiesMeshesOverlapped[indexEnemy]->SetScalarParameterValueOnMaterials("Z value", 1.0f);
					EnemiesMeshesOverlapped.Remove(skeletalMesh);
				}
			}
		}
	}
}

void ABlackHole::LaunchSpell(FVector _direction)
{
	IsLaunch = true;
	CanGrow = true;
	LastPosition = GetActorLocation();
	if (ProjectileMovement != nullptr)
		ProjectileMovement->Velocity = _direction * Speed;
	CollisionComp->SetCollisionProfileName(TEXT("ProjectileWithoutColl"));
	TriggerComp->SetCollisionProfileName(TEXT("TriggerDmg"));
	SetActorRotation(FQuat::Identity);
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
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::BeginOverlapHit);
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

		for (int i = 0; i < EnemiesMeshesOverlapped.Num(); ++i)
		{
			EnemiesMeshesOverlapped[i]->SetVectorParameterValueOnMaterials("Location", GetActorLocation());
		}
	}

	if (CanGrow)
	{
		for (int i = 0; i < EnemiesMeshesOverlapped.Num(); ++i)
		{
			if (TimersAbsorption[i] > 0.0f)
			{
				TimersAbsorption[i] = FMath::Clamp(TimersAbsorption[i] - _deltaTime * 0.3f, 0.0f, 1.0f);
				EnemiesMeshesOverlapped[i]->SetScalarParameterValueOnMaterials("Z value", TimersAbsorption[i]);
			}
			else
			{
					EnemiesOverlapped[i]->InstaKill();
			}
		}
		if (CollisionComp->GetComponentScale().X < 6.0f)
		{
			CollisionComp->SetWorldScale3D(CollisionComp->GetComponentScale() + FVector(_deltaTime * 1.25f, _deltaTime * 1.25f, _deltaTime * 1.25f));
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

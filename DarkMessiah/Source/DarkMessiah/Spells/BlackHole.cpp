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
	WaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaveMesh"));
	DistorsionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DistorsionMesh"));
	SecondRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SecondRoot"));
	RootComponent = CollisionComp;
	TriggerComp->SetupAttachment(CollisionComp);
	SecondRootComponent->SetupAttachment(CollisionComp);

	MeshComponent->SetupAttachment(SecondRootComponent);
	WaveMesh->SetupAttachment(SecondRootComponent);
	DistorsionMesh->SetupAttachment(SecondRootComponent);
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
				HelperLibrary::Print("cc");
				if (skeletalMesh->GetMaterial(0)->GetName() == "M_UE4Man_Body_Distortion_Inst")
				{
					EnemiesMeshesOverlapped.Add(skeletalMesh);
					TimersAbsorption.Add(1.0f);
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
		for (int i = 0; i < EnemiesMeshesOverlapped.Num(); ++i)
		{
			EnemiesMeshesOverlapped[i]->SetVectorParameterValueOnMaterials("Location", GetActorLocation());
		}
	}

	if (CanGrow)
	{
		for (int i = 0; i < EnemiesMeshesOverlapped.Num(); ++i)
		{
			HelperLibrary::Print(FString::FromInt(i) + " " + FString::SanitizeFloat(TimersAbsorption[i]));
			if (TimersAbsorption[i] > 0.0f)
			{
				TimersAbsorption[i] -= _deltaTime * 0.3f;
				EnemiesMeshesOverlapped[i]->SetScalarParameterValueOnMaterials("Z value", TimersAbsorption[i]);
			}
		}
		if (SecondRootComponent->GetComponentScale().X < 6.0f)
		{
			SecondRootComponent->SetWorldScale3D(SecondRootComponent->GetComponentScale() + FVector(_deltaTime * 1.25f, _deltaTime * 1.25f, _deltaTime * 1.25f));
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "IceSpike.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Helpers/HelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

AIceSpike::AIceSpike(const FObjectInitializer& _objectInit)
{
	sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = sceneComp;
	Mesh = _objectInit.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MESH"));
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp")); 
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	CollisionComp->SetupAttachment(RootComponent);
	Mesh->SetupAttachment(CollisionComp);
}


void AIceSpike::LaunchSpell(FVector _direction)
{
	GetProjectileMovement()->Velocity = _direction * Speed;
	SetLifeSpan(2.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Projectile"));
}

void AIceSpike::InitSpell()
{

}

void AIceSpike::ImpaleActor(const FHitResult& _hitStaticResult, const FHitResult& _hitPawnResult)
{
	if (UWorld* world = GetWorld())
	{
		FVector direction;
		FVector readjustPosition;
		//spawn an actor with PhysicsConstraint to stuck pawn in a wall
		ImpalementActor = world->SpawnActor<UPhysicsConstraintComponent>(Impalement);
		if (ImpalementActor != nullptr)
		{
			FAttachmentTransformRules attachementToActor(EAttachmentRule::KeepWorld, false);
			ImpalementActor->AttachToComponent(_hitPawnResult.GetComponent(), attachementToActor, _hitPawnResult.BoneName);

			//readjust position of pawn to avoid it to thrill in the wall
			direction = (GetActorLocation() - _hitStaticResult.ImpactPoint);
			direction /= direction.Size();
			readjustPosition = _hitStaticResult.ImpactPoint + direction * 5.0f;
			SetActorLocation(readjustPosition);

			SetLifeSpan(10.0f);

			//place the physics constraint on the wall to keep player stuck to the wall
			ImpalementActor->SetWorldLocation(_hitStaticResult.ImpactPoint);
			ImpalementActor->SetConstrainedComponents(_hitStaticResult.GetComponent(), _hitStaticResult.BoneName, _hitPawnResult.GetComponent(), _hitPawnResult.BoneName);
		}
	}
}

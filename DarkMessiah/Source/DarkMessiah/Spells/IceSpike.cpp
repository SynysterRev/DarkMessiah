// Fill out your copyright notice in the Description page of Project Settings.


#include "IceSpike.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Helpers/HelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "CharacterAI.h"
#include "DarkMessiahCharacter.h"
#include "Components/SkeletalMeshComponent.h"

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


void AIceSpike::ImpaleActor(const FHitResult& _hitStaticResult, const FHitResult& _hitPawnResult, AActor* OtherActor, FVector _velocity)
{
	if (UWorld* world = GetWorld())
	{
		FVector direction;
		FVector readjustPosition;
		//spawn an actor with PhysicsConstraint to stuck pawn in a wall
		ImpalementComponent = world->SpawnActor<AActor>(Impalement)->FindComponentByClass<UPhysicsConstraintComponent>();
		if (ImpalementComponent != nullptr)
		{
			//physicsconstrait attach to the pawn
			FAttachmentTransformRules attachementToActor(EAttachmentRule::KeepWorld, false);
			ImpalementComponent->AttachToComponent(_hitPawnResult.GetComponent(), attachementToActor, _hitPawnResult.BoneName);
			if (ACharacterAI* ai = Cast<ACharacterAI>(OtherActor))
			{
				MeshHit = ai->GetMesh();
				direction = _velocity / _velocity.Size();
				if (MeshHit)
				{
					//move the mesh in the same direction as the spike
					MeshHit->SetAllPhysicsLinearVelocity(direction * 8000.0f);
				}
			}
			SetLifeSpan(TimerBeforeDestruction);
			world->GetTimerManager().SetTimer(TimerDestruction, this, &AIceSpike::DestroyImpalement, TimerBeforeDestruction, true);

			//place the physics constraint on the wall to keep player stuck to the wall
			/*ImpalementComponent->SetWorldLocation(_hitStaticResult.ImpactPoint);
			ImpalementComponent->SetConstrainedComponents(_hitStaticResult.GetComponent(), _hitStaticResult.BoneName, _hitPawnResult.GetComponent(), _hitPawnResult.BoneName);*/
		}
	}
}

void AIceSpike::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		FVector velocity = GetProjectileMovement()->Velocity;
		//attach spike to the actor hit
		FAttachmentTransformRules attachementToActor(EAttachmentRule::KeepWorld, false);
		AttachToComponent(OtherComp, attachementToActor, Hit.BoneName);

		ActorHit = Cast<ACharacterAI>(OtherActor);

		if (ActorHit != nullptr)
		{
			FHitResult hitResult;
			FVector endLine = GetActorForwardVector() * DistanceImpalement;
			FCollisionQueryParams collisionQueryParems;
			BoneHit = Hit.BoneName;
			ComponentHit = OtherComp;
			if (UWorld* world = GetWorld())
			{
				//inflict damage to the ai
				if (Caster)
				{
					FDamageEvent damageEvent;
					ActorHit->TakeDamage(Damage, damageEvent, Caster->GetController(), this);
				}
				PointImpactOnPawn = Hit.ImpactPoint;
				//raycast to see if there's a wall behind ai
				bool hit = world->LineTraceSingleByObjectType(hitResult, Hit.ImpactPoint, endLine, ECC_WorldStatic, collisionQueryParems);
				if (hit)
				{
					PointImpactOnStatic = hitResult.ImpactPoint;
					if (ActorHit->IsCharacterDead())
					{
						HasImpaled = true;
						ImpaleActor(hitResult, Hit, OtherActor, velocity);
					}
				}
			}
		}
	}
	else
	{
		Destroy();
	}
}

void AIceSpike::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddDynamic(this, &AIceSpike::OnHit);
}

void AIceSpike::DestroyImpalement()
{
	Destroy();
	if (ImpalementComponent != nullptr)
	{
		ImpalementComponent->DestroyComponent();
	}
}

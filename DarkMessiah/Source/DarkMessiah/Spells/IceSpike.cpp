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
			FAttachmentTransformRules attachementToActor(EAttachmentRule::KeepWorld, false);
			ImpalementComponent->AttachToComponent(_hitPawnResult.GetComponent(), attachementToActor, _hitPawnResult.BoneName);
			if (ACharacterAI* ai = Cast<ACharacterAI>(OtherActor))
			{
				MeshHit = ai->GetMesh();
				//direction = _velocity / _velocity.Size();
				
				direction = _hitStaticResult.ImpactPoint - _hitPawnResult.ImpactNormal;// + direction * 5.0f;
				direction /= direction.Size();
				ai->SetImpaled(true);
				ai->SetConstraint(ImpalementComponent);
				if (MeshHit)
				{
					MeshHit->SetAllPhysicsLinearVelocity(direction * 8000.0f);
				}
			}
			//readjust position of pawn to avoid it to thrill in the wall
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
				if (Caster)
				{
					FDamageEvent damageEvent;
					ActorHit->TakeDamage(Damage, damageEvent, Caster->GetController(), this);
				}
				PointImpactOnPawn = Hit.ImpactPoint;
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

void AIceSpike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasImpaled)
	{
		if (UWorld* world = GetWorld())
		{
			FHitResult hitResult;
			FVector endLine = GetActorForwardVector() * DistanceImpalement;
			FCollisionQueryParams collisionQueryParems;
			HelperLibrary::Print(FString::SanitizeFloat(FVector::Distance(MeshHit->GetComponentLocation(), PointImpactOnStatic)));
			if (FVector::Distance(MeshHit->GetComponentLocation(), PointImpactOnStatic) < 200.0f)
			{
				FVector test = MeshHit->GetComponentLocation() - PointImpactOnStatic;
				test /= test.Size();
				FVector adjust = PointImpactOnStatic + test * 20.0f;
				MeshHit->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
				ComponentHit->SetWorldLocation(adjust);
				HelperLibrary::Print("cccc");
				ImpalementComponent->SetWorldLocation(PointImpactOnStatic);
				ImpalementComponent->SetConstrainedComponents(hitResult.GetComponent(), hitResult.BoneName, ComponentHit, BoneHit);
				HasImpaled = false;
			}
		}
	}
}

void AIceSpike::DestroyImpalement()
{
	Destroy();
	if (ImpalementComponent != nullptr)
	{
		ImpalementComponent->DestroyComponent();
	}
}

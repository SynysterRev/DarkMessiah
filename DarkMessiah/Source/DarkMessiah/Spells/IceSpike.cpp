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
#include "CollisionQueryParams.h"
#include "Engine/World.h"

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
	MeshHit = nullptr;
	bStopSpell = false;
}


void AIceSpike::LaunchSpell(FVector _direction)
{
	DirVelocity = _direction * Speed;
	GetProjectileMovement()->Velocity = DirVelocity;
	SetLifeSpan(2.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Projectile"));
}


void AIceSpike::ImpaleActor(const FHitResult& _hitStaticResult, const FHitResult& _hitPawnResult, AActor* OtherActor, FVector _velocity)
{
	/*if (UWorld* world = GetWorld())
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
			/*}
		}*/
}

void AIceSpike::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		FVector velocity = GetProjectileMovement()->Velocity;
		//attach spike to the actor hit


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

				if (ActorHit->IsCharacterDead())
				{
					MeshHit = ActorHit->GetMesh();

					//the bone hit becomes the heaviest bone
					MeshHit->SetAllMassScale(0.0f);
					MeshHit->SetMassScale(BoneHit);
				}

				FAttachmentTransformRules attachementToActor(EAttachmentRule::KeepWorld, false);
				AttachToComponent(OtherComp, attachementToActor, Hit.BoneName);
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

void AIceSpike::Tick(float _deltaTime)
{
	if (MeshHit)
	{
		if (!bStopSpell)
		{
			//Attached Bone Follow Spell
			GetProjectileMovement()->Velocity = DirVelocity;
			MeshHit->SetPhysicsLinearVelocity(DirVelocity, false, BoneHit);

			FHitResult hit;
			if (GetWorld()->LineTraceSingleByChannel(hit, Mesh->GetComponentLocation(), Mesh->GetComponentLocation() + (DirVelocity / DirVelocity.Size())*7.0f, ECollisionChannel::ECC_GameTraceChannel4))
			{
				bStopSpell = true;

				GetProjectileMovement()->StopMovementImmediately();
				SetLifeSpan(TimerBeforeDestruction);

				bonePos = hit.Location + hit.Normal * 10.0f;
				MeshHit->SetAllMassScale(1.0f);
			}
		}
		else
		{
			//to do make a better stability pos
			FTransform tr(bonePos);
			MeshHit->GetBodyInstance(BoneHit)->SetBodyTransform(tr, ETeleportType::None);
			MeshHit->GetBodyInstance(BoneHit)->SetLinearVelocity(FVector::ZeroVector, false);
		}
	}
}

void AIceSpike::DestroyImpalement()
{
	HelperLibrary::Print("stop2");
	Destroy();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "IceSpike.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Helpers/HelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"

AIceSpike::AIceSpike(const FObjectInitializer& _objectInit)
{
	sceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Test"));
	RootComponent = sceneComp;
	Mesh = _objectInit.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("MESH"));
	Mesh->SetupAttachment(RootComponent);
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp")); 
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = Mesh;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	CollisionComp->SetupAttachment(Mesh);
}


void AIceSpike::LaunchSpell(FVector _direction)
{
	GetProjectileMovement()->Velocity = _direction * Speed;
	SetLifeSpan(2.0f);
	GetMesh()->SetCollisionProfileName(TEXT("Projectile"));
}

void AIceSpike::InitSpell()
{
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

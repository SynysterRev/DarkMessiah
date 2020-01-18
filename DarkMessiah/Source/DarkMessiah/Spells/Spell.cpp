// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Engine/Engine.h>
#include "DarkMessiahCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ASpell::ASpell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	RootComponent = CollisionComp;
}

// Called when the game starts or when spawned
void ASpell::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*void ASpell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != nullptr && OtherActor != this && Cast<ADarkMessiahCharacter>(OtherActor) == nullptr)
	{
		//UKismetSystemLibrary::SphereOverlapActors()
		//OtherComp->AddRadialImpulse(Hit.Location, m_radiusExplosion, m_strength, ERadialImpulseFalloff::RIF_Constant, true);
		Destroy();
	}
}*/

void ASpell::LaunchSpell(FVector _direction)
{
	GetProjectileMovement()->Velocity = _direction * Speed;
	SetLifeSpan(2.0f);
	GetSphereComponent()->SetCollisionProfileName(TEXT("Projectile"));
	//CollisionComp->OnComponentHit.AddDynamic(this, &ASpell::OnHit);		// set up a notification for when this component hits something blocking
}

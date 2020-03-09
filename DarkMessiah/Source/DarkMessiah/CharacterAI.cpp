// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Helpers/HelperLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ACharacterAI::ACharacterAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetupAttachment(GetCapsuleComponent());
	//RootComponent = Root;
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	//GetMesh()->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ACharacterAI::BeginPlay()
{
	Super::BeginPlay();
	IsRagdollActivate = false;
	HealthComponent = FindComponentByClass<UHealthComponent>();
	GetMesh()->OnComponentHit.AddDynamic(this, &ACharacterAI::OnHit);
}

void ACharacterAI::ActivateRagDoll()
{
	if (!IsRagdollActivate)
	{
		GetCharacterMovement()->StopMovementImmediately();
		if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent())
		{
			//CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CapsuleComp->DestroyComponent();
		}
		RootComponent = Root;
		GetMesh()->SetupAttachment(Root);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetSimulatePhysics(true);
		IsRagdollActivate = true;
	}
}

void ACharacterAI::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{

	}
}

void ACharacterAI::TakeDamage(const int32 Damage, class AActor* _damageCauser)
{
	if (HealthComponent == nullptr || HealthComponent->IsCharacterDead()) return;

	if (Damage > 0.0f)
	{
		HealthComponent->InflictDamage(Damage);
		OnCharacterTakeDamage.Broadcast(Damage);
		Event_OnTakeDamage_BP(Damage);
		if (HealthComponent->IsCharacterDead())
		{
			ActivateRagDoll();
			OnCharacterDied.Broadcast();
		}
	}
}

void ACharacterAI::SlowCharacter(float _percentageSlow)
{
	if (GetCharacterMovement())
		GetCharacterMovement()->MaxWalkSpeed -= GetCharacterMovement()->MaxWalkSpeed * _percentageSlow;
}

void ACharacterAI::SpeedUpCharacter(float _percentageSpeedUp)
{
	if (GetCharacterMovement() && _percentageSpeedUp < 1.0f)
		GetCharacterMovement()->MaxWalkSpeed += GetCharacterMovement()->MaxWalkSpeed * _percentageSpeedUp;
	else if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed *= _percentageSpeedUp;
	}
}

// Called every frame
void ACharacterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "DarkMessiahCharacter.h"
#include "DarkMessiahProjectile.h"
#include "Spells/Spell.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Engine/Engine.h"
#include "Spells/FireBall.h"
#include "Spells/IceSpike.h"
#include "Helpers/HelperLibrary.h"
#include "HealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADarkMessiahCharacter

ADarkMessiahCharacter::ADarkMessiahCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	SpellOffset = CreateDefaultSubobject<USceneComponent>(TEXT("OffsetSpell"));
	SpellOffset->SetupAttachment(Mesh1P);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.


	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void ADarkMessiahCharacter::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	Mesh1P->SetHiddenInGame(false, true);

	HealthComponent = FindComponentByClass<UHealthComponent>();
	// try and fire a projectile
	TypeSpell = ETypeSpell::FireBall;
	CreateLaunchingSpell();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ADarkMessiahCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ADarkMessiahCharacter::PrepareFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ADarkMessiahCharacter::OnFire);

	PlayerInputComponent->BindAction("SelectSpell1", IE_Pressed, this, &ADarkMessiahCharacter::ChangeSpell1);
	PlayerInputComponent->BindAction("SelectSpell2", IE_Pressed, this, &ADarkMessiahCharacter::ChangeSpell2);


	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ADarkMessiahCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADarkMessiahCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADarkMessiahCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADarkMessiahCharacter::LookUpAtRate);
}

void ADarkMessiahCharacter::OnFireSpell_Implementation(ASpell* _spell)
{
}

void ADarkMessiahCharacter::OnFire()
{
	if (spell != nullptr && IsCasting)
	{
		if (UWorld* world = GetWorld())
		{
			FHitResult hitResult;
			FCollisionQueryParams collisionQueryParems;
			FVector end = FirstPersonCameraComponent->GetComponentLocation() + FirstPersonCameraComponent->GetForwardVector() * 20000.0f;
			bool hit = world->LineTraceSingleByChannel(hitResult, FirstPersonCameraComponent->GetComponentLocation(), end,
				ECC_Visibility, collisionQueryParems);
			FVector direction;
			direction = hit ? (hitResult.ImpactPoint - spell->GetActorLocation()).GetSafeNormal() : end;
			FDetachmentTransformRules detachementParam(EDetachmentRule::KeepWorld, false);
			spell->DetachFromActor(detachementParam);
			spell->LaunchSpell(direction);
			OnFireSpell(spell);
			spell = nullptr;
			IsCasting = false;
			FTimerHandle m_timerSpawnFireBall;
			world->GetTimerManager().SetTimer(m_timerSpawnFireBall, this, &ADarkMessiahCharacter::CreateLaunchingSpell, CDSpawnSpell, false);
		}
	}


	// try and play a firing animation if specified
	//if (FireAnimation != NULL)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != NULL)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}
}

void ADarkMessiahCharacter::CreateLaunchingSpell()
{
	if (FireSpell != NULL && spell == nullptr && IceSpell != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			FRotator SpawnRotation = GetControlRotation();
			if (SpellOffset != nullptr)
			{
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = SpellOffset->GetComponentLocation();
				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;

				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
				// spawn the projectile at the muzzle

				if (TypeSpell == ETypeSpell::FireBall)
					spell = World->SpawnActor<AFireBall>(FireSpell, SpawnLocation, SpawnRotation, ActorSpawnParams);

				else if (TypeSpell == ETypeSpell::IceSpike)
				{
					spell = World->SpawnActor<AIceSpike>(IceSpell, SpawnLocation, SpawnRotation, ActorSpawnParams);
				}
				//spell = static_cast<AFireBall*>(spell);
				if (spell != nullptr)
				{
					FAttachmentTransformRules attachementPawn(EAttachmentRule::KeepWorld, false);
					spell->AttachToComponent(SpellOffset, attachementPawn);
					spell->InitSpell();
					spell->Caster = this;
				}
			}
		}
	}
}

void ADarkMessiahCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADarkMessiahCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADarkMessiahCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADarkMessiahCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADarkMessiahCharacter::PrepareFire()
{
	if (spell != nullptr)
	{
		spell->PrepareSpell();
		IsCasting = true;
	}
}

/*void ADarkMessiahCharacter::PrepareSecondFire()
{
	if (spell != nullptr)
	{
		spell->PrepareSecondSpell();
		IsCasting = true;
	}
}*/

void ADarkMessiahCharacter::ChangeSpell1()
{
	if (TypeSpell != ETypeSpell::FireBall)
	{
		TypeSpell = ETypeSpell::FireBall;
		ClearSpell();
		CreateLaunchingSpell();
	}
}

void ADarkMessiahCharacter::ChangeSpell2()
{
	if (TypeSpell != ETypeSpell::IceSpike)
	{
		TypeSpell = ETypeSpell::IceSpike;
		ClearSpell();
		CreateLaunchingSpell();
	}
}

void ADarkMessiahCharacter::ClearSpell()
{
	if (spell != nullptr)
	{
		FDetachmentTransformRules detachementParam(EDetachmentRule::KeepWorld, false);
		spell->DetachFromActor(detachementParam);
		spell->DestroySpell();
		spell = nullptr;
	}
}

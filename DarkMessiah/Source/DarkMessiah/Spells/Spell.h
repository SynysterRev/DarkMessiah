// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

UCLASS()
class DARKMESSIAH_API ASpell : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpell();

protected:
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, Category = Projectile)
		class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float Speed;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float RadiusExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float Strength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		FVector  MaxScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		FVector  AddingScale;

	UPROPERTY()
		float PowerMultiplicator;

	UPROPERTY()
		float CurrentPowerMultiplicator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float MaxPowerMultiplicator;

	UPROPERTY()
		FVector CurrentScale;

	UPROPERTY()
		FVector CurrentLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float TimerIncrease;

	FTimerHandle TimerPreparation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** called when projectile hits something */
	/*UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

	UFUNCTION(BlueprintCallable)
		void LaunchSpell(FVector _direction);
	UFUNCTION()
		void PrepareLaunch();
	UFUNCTION()
		void IncreasePower();

	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	FORCEINLINE class USphereComponent* GetSphereComponent() const { return CollisionComp; }

};

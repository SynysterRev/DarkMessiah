// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Spell.h"
#include "FireBall.generated.h"

/**
 * 
 */
class ASpell;
UCLASS()
class DARKMESSIAH_API AFireBall : public ASpell
{
	GENERATED_BODY()

		AFireBall();

public :
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float TimerIncrease;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float MaxPowerMultiplicator;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		FVector  MaxScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		FVector  AddingScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float RadiusExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		float Strength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	void Blblbl();
private :
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, Category = Projectile)
		class USphereComponent* CollisionComp;
	UPROPERTY()
		float PowerMultiplicator;
	UPROPERTY()
		float CurrentPowerMultiplicator;
	FTimerHandle TimerPreparation;
	UFUNCTION()
		void IncreasePower();
	void LaunchSpell(FVector _direction) override;
	void PrepareSpell() override;

	FORCEINLINE class USphereComponent* GetSphereComponent() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Spell.h"
#include "FireBall.generated.h"

/**
 * 
 */
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
	FVector MaxScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	FVector AddingScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float RadiusExplosion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float Strength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float MaxStrength = 2000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = Projectile)
	FORCEINLINE float GetStrengthPercent() { return (Strength / MaxStrength); };
private :
	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class USphereComponent* CollisionComp;
	UPROPERTY()
	float PowerMultiplicator;
	UPROPERTY()
	float CurrentPowerMultiplicator;

	void BeginPlay() override;

	FTimerHandle TimerPreparation;
	UFUNCTION()
	void IncreasePower();
	void LaunchSpell(FVector _direction) override;
	void PrepareSpell() override;

	FORCEINLINE class USphereComponent* GetSphereComponent() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

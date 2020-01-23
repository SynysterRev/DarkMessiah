// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Spell.h"
#include "IceSpike.generated.h"

/**
 * 
 */
UCLASS()
class DARKMESSIAH_API AIceSpike : public ASpell
{
	GENERATED_BODY()
		AIceSpike(const FObjectInitializer& _objectInit);
public :
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Projectile)
	float Strength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	void LaunchSpell(FVector _direction) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
	class UStaticMeshComponent* Mesh;
private:
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UCapsuleComponent* CollisionComp;

	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

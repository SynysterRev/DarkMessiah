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


	void LaunchSpell(FVector _direction) override;

	void InitSpell() override;

	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	UFUNCTION(BlueprintCallable)
	void ImpaleActor(const FHitResult& _hitStaticResult, const FHitResult& _hitPawnResult);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPhysicsConstraintComponent> Impalement;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class USceneComponent* sceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionComp;

	UPhysicsConstraintComponent* ImpalementActor;

};

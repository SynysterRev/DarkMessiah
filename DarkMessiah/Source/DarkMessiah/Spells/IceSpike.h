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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UStaticMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE class UCapsuleComponent* GetCapsuleComponent() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void PrepareSecondSpell() override;

	UPROPERTY(EditAnywhere)
	int32 NumberMaxSpike;

	UPROPERTY(EditAnywhere)
	float TimerCreation;

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AIceSpike> IceSpike;

	UPROPERTY(EditAnywhere)
	float DistanceImpalement;

	UPROPERTY(EditAnywhere)
	int32 Damage;

	UPROPERTY(EditAnywhere)
	float TimerBeforeDestruction;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	void BeginPlay() override;
	void Tick(float _deltaTime) override;

	void CreateSpike();

	TArray<AIceSpike*> AllProjectile;

	FTimerHandle TimerDestruction;
	FTimerHandle TimerSpawn;

	bool HasImpaled;
	bool bStopSpell;

	int32 CurrentNumberSpike;

	FVector PointImpactOnPawn;
	FVector PointImpactOnStatic;
	FVector DirVelocity;
	FVector DirHit;
	FVector bonePos;

	FName BoneHit;
	class UPrimitiveComponent* ComponentHit;
	class USkeletalMeshComponent* MeshHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class USceneComponent* sceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionComp;

	class UPhysicsConstraintComponent* ImpalementComponent;
	class ACharacterAI* ActorHit;
};

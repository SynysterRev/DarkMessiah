// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spells/Spell.h"
#include "BlackHole.generated.h"

/**
 * 
 */
UCLASS()
class DARKMESSIAH_API ABlackHole : public ASpell
{
	GENERATED_BODY()

	ABlackHole();

protected:
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BeginOverlapHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = Projectile)
	class USphereComponent* TriggerComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* WaveMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* DistorsionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* SecondRootComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	class UActorComponent* DistorsionActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Damage)
	float Damage;

	UPROPERTY(EditAnywhere)
	float MaxDistance;

	UPROPERTY(EditAnywhere)
	float TimerDamage;

	UPROPERTY(EditAnywhere)
	float SpanLife;

	UPROPERTY(EditAnywhere)
	float TimeSuction;

	UPROPERTY(EditAnywhere)
	float MaxSize;

	UPROPERTY(EditAnywhere)
	float PercentageSlow;

	void LaunchSpell(FVector _direction) override;

	void BeginPlay() override;

	void Tick(float _deltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = Gameplay)
		bool CanGrow;

private:
	UPROPERTY()
	float CurrentDistance;
	
	UPROPERTY()
	bool IsLaunch;

	TArray<class ACharacterAI*> EnemiesOverlapped;

	UPROPERTY(VisibleAnywhere)
	TArray<class USkeletalMeshComponent*> EnemiesMeshesOverlapped;

	TArray<float> TimersAbsorption;

	FVector LastPosition;

	void InflictDamage();
};

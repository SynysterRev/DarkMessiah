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

protected:

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float Speed;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		float MaxSpeed;

	UPROPERTY()
		FVector CurrentScale;

	UPROPERTY()
		FVector CurrentLocation;

public:

	/** called when projectile hits something */
	/*UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

	UFUNCTION(BlueprintCallable)
		virtual void LaunchSpell(FVector _direction);

	UFUNCTION()
		virtual void InitSpell();

	UFUNCTION()
		virtual void PrepareSpell();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "CharacterAI.generated.h"

UCLASS()
class DARKMESSIAH_API ACharacterAI : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Root;
public:
	// Sets default values for this character's properties
	ACharacterAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ActivateRagDoll();

	UPROPERTY(BlueprintReadOnly)
		bool IsRagdollActivate;

	UPROPERTY()
	bool IsImpaled;

	class UPhysicsConstraintComponent* ImpalementComponent;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	UFUNCTION(BlueprintCallable)
	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	FORCEINLINE bool IsCharacterDead() const { return HealthComponent->IsCharacterDead(); }

	inline void SetImpaled(bool _isImpaled) { IsImpaled = _isImpaled; }

	inline void SetConstraint(class UPhysicsConstraintComponent* _constraint) { ImpalementComponent = _constraint; }
};

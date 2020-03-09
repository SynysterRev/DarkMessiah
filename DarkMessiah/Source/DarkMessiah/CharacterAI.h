// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthComponent.h"
#include "CharacterAI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDiedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterTakeDamageDelegate, int32, _damage);
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

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	UPROPERTY(BlueprintAssignable, Category = "Death")
	FCharacterDiedDelegate OnCharacterDied;

	UPROPERTY(BlueprintAssignable, Category = "Damage")
	FCharacterTakeDamageDelegate OnCharacterTakeDamage;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(const int32 Damage, class AActor* _damageCauser);

	UFUNCTION(BlueprintImplementableEvent)
		void Event_OnTakeDamage_BP(int32 _damageReceived);

	UFUNCTION()
	void SlowCharacter(float _percentageSlow);

	UFUNCTION()
	void SpeedUpCharacter(float _percentageSpeedUp);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UHealthComponent* GetHealthComponent() const { return HealthComponent; }

	FORCEINLINE bool IsCharacterDead() const { return HealthComponent->IsCharacterDead(); }
};

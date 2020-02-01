// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKMESSIAH_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 MaxHealth;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetLife() { return Health; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMaxLife() { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCharacterDead() { return IsDead; }

	UFUNCTION(BlueprintCallable)
	void InflictDamage(int32 _damage);

	UFUNCTION(BlueprintImplementableEvent)
	void Event_InflictDamageBP(int32 _damage);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private :
	int32 Health;

	bool IsDead;
};

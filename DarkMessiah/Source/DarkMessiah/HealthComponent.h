// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterLoseHealthDelegate, int32, _damage);

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
	FORCEINLINE float GetPercentHealth() { return (float)Health / (float)MaxHealth; }

	//UPROPERTY(EditAnywhere)
	/*TSubclassOf<UTextRenderComponent> */

public:	
	UPROPERTY(BlueprintAssignable, Category = "Damage Taken")
		FCharacterLoseHealthDelegate OnCharacterTakeDamage;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetMaxLife() { return MaxHealth; }
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void InflictDamage(int32 _damage);	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCharacterDead() { return IsDead; }

private :
	int32 Health;

	bool IsDead;
};

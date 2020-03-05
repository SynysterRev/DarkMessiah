// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Engine/Engine.h>
#include "DarkMessiahCharacter.h"
#include "Kismet/KismetSystemLibrary.h"


void ASpell::LaunchSpell(FVector _direction)
{

}

void ASpell::InitSpell(ADarkMessiahCharacter* _caster)
{
	Caster = _caster;
}

void ASpell::PrepareSpell()
{
}

void ASpell::PrepareSecondSpell()
{
}

void ASpell::DestroySpell()
{
	Destroy();
}



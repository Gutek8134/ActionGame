// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GA_ItemAbility.h"
#include "GA_ItemCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UGA_ItemCombatAbility : public UGA_ItemAbility
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);
};

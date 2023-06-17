// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ItemCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameTypes.h"
#include "ActionGameCharacter.h"

FGameplayEffectSpecHandle UGA_ItemCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult) {
	if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo()) {
		if (const UStaticWeaponData* WeaponStaticData = GetEquippedWeaponStaticData()) {
			FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilityComponent->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);
			
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);
				
			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}
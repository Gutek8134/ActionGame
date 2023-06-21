// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ItemCombatAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "ActionGameTypes.h"
#include "ActionGameCharacter.h"

#include "Inventory/ItemActors/WeaponActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"

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

const bool UGA_ItemCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult)
{
	AWeaponActor* WeaponActor = GetEquippedWeaponItemActor();

	AActionGameCharacter* ActionGameCharacter = GetActionGameCharacterFromActorInfo();

	const FTransform& CameraTransform = ActionGameCharacter->GetFollowCamera()->GetComponentTransform();

	const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;

	TArray<AActor*> ActorsToIgnore = { GetAvatarActorFromActorInfo() };
	
	FHitResult FocusHit;

	UKismetSystemLibrary::LineTraceSingle(this, CameraTransform.GetLocation(), FocusTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, FocusHit, true);
	
	FVector MuzzleLocation = WeaponActor->GetMuzzleLocation();

	const FVector WeaponTraceEnd = MuzzleLocation + (FocusHit.Location - MuzzleLocation).GetSafeNormal() * TraceDistance;

	UKismetSystemLibrary::LineTraceSingle(this, MuzzleLocation, WeaponTraceEnd, TraceType, false, ActorsToIgnore, EDrawDebugTrace::None, OutHitResult, true);

	return OutHitResult.bBlockingHit;
}

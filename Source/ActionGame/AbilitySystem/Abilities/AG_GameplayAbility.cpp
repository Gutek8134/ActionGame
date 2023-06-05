// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "ActionGameCharacter.h"

AActionGameCharacter* UAG_GameplayAbility::GetActionGameCharacterFromActorInfo() const
{

	return Cast<AActionGameCharacter>(GetAvatarActorFromActorInfo());
}

void UAG_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();

	if (auto AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		for (auto const& GameplayEffect : OngoingEffectsToJustApplyOnStart) {
			if (!GameplayEffect.Get()) continue;
			FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

			if (SpecHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied()) {
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
				}
			}
		}
		if (IsInstantiated()) {
			for (const auto& GameplayEffect : OngoingEffectsToRemoveOnEnd) {
				if (!GameplayEffect.Get()) continue;
				FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

				if (SpecHandle.IsValid()) {
					FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied()) {
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
					}
					else {
						RemoveOnEndEffectHandles.Add(ActiveGEHandle);
					}
				}
			}

		}
	}
}


void UAG_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated()) {
		if (auto AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get()) {
			for (const auto& ActiveEffectHandle : RemoveOnEndEffectHandles) {
				if (ActiveEffectHandle.IsValid()) {
					AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
				}
			}
		}

		RemoveOnEndEffectHandles.Empty();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

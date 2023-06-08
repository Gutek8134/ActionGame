// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Jump.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DrawDebugHelpers.h"

UGA_Jump::UGA_Jump() {
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UGA_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	const bool bMovementAllowsJump = Character->GetCharacterMovement()->IsJumpAllowed();

	UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
	const bool bIsWallrunning = AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WallrunStateTag);

	return Character->CanJump() || (bMovementAllowsJump && bIsWallrunning);
}

void UGA_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

		ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());

		UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Character);
		if (AbilitySystemComponent->HasMatchingGameplayTag(WallrunStateTag)) {
			FGameplayTagContainer WallrunTags(WallrunStateTag);

			AbilitySystemComponent->CancelAbilities(&WallrunTags);

			FVector JumpOffVector = Character->GetCharacterMovement()->GetCurrentAcceleration().GetSafeNormal() + FVector::UpVector;

			Character->LaunchCharacter(JumpOffVector * OffWallJumpStrength, true, true);
		}
		else
			Character->Jump();
	}
}

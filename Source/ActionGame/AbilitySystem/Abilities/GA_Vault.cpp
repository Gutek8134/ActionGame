// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Vault.h"
#include "AbilitySystem/Abilities/GA_Vault.h"
#include "ActionGameCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"

UGA_Vault::UGA_Vault()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Vault::CommitCheck(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	if (!Super::CommitCheck(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags)) return false;

	AActionGameCharacter* Character = CastChecked<AActionGameCharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	if (!IsValid(Character)) return false;

	const FVector StartLocation = Character->GetActorLocation();
	const FVector ForwardVector = Character->GetActorForwardVector();
	const FVector UpVector = Character->GetActorUpVector();

	TArray<AActor*> ActorsToIgnore = { Character };

	static const auto Cvar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));
	const bool bShowTraversal = Cvar->GetInt() > 0;

	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	bool bJumpToLocationSet = false;

	int32 JumpToLocationIdx = INDEX_NONE;

	FHitResult TraceHit;
	float MaxJumpDistance = HorizontalTraceLength;

	int i = 0;
	for (; i < HorizontalTraceCount; ++i) {
		const FVector TraceStart = StartLocation + i * UpVector * HorizontalTraceStep;
		const FVector TraceEnd = TraceStart + ForwardVector * HorizontalTraceLength;
		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true)) {
			if (JumpToLocationIdx == INDEX_NONE && (i < HorizontalTraceCount - 1)) {
				JumpToLocationIdx = i;
				JumpToLocation = TraceHit.Location;
			}
			else if (JumpToLocationIdx == (i - 1)) {
				MaxJumpDistance = FVector::Dist2D(TraceHit.Location, TraceStart);
				break;
			}
			else if (JumpToLocationIdx != INDEX_NONE) {
				break;
			}
		}
	}

	if (JumpToLocationIdx == INDEX_NONE) return false;

	const float DistanceToJumpTo = FVector::Dist2D(StartLocation, JumpToLocation);
	const float MaxVerticalTraceDistance = MaxJumpDistance - DistanceToJumpTo;

	if (MaxVerticalTraceDistance < 0) return false;

	if (i == HorizontalTraceCount) {
		i = HorizontalTraceCount - 1;
	}

	const float VerticalTraceLength = FMath::Abs(JumpToLocation.Z - (StartLocation + i * UpVector * HorizontalTraceStep).Z);
	FVector VerticalStartLocation = JumpToLocation + UpVector * VerticalTraceLength;

	i = 0;

	const float VerticalTraceCount = MaxVerticalTraceDistance / VerticalTraceStep;
	bool bJumpOverLocationSet = false;

	for (; i <= VerticalTraceCount; ++i) {
		const FVector TraceStart = VerticalStartLocation + i * ForwardVector * VerticalTraceStep;
		const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1;

		if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true)) {
			JumpOverLocation = TraceHit.ImpactPoint;

			if (i == 0) {
				JumpToLocation = JumpOverLocation;
			}
		}
		else if (i != 0) {
			bJumpOverLocationSet = true;
			break;
		}
	}

	if (!bJumpOverLocationSet) return false;

	const FVector TraceStart = JumpOverLocation + ForwardVector * VerticalTraceStep;
	const FVector TraceEnd = TraceStart + UpVector * VerticalTraceLength * -1;
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(this, TraceStart, TraceEnd, HorizontalTraceRadius, TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, TraceHit, true)) {
		JumpOverLocation = TraceHit.ImpactPoint;
	}

	if (bShowTraversal) {
		DrawDebugSphere(GetWorld(), JumpToLocation, 15, 16, FColor::White, false, 7);
		DrawDebugSphere(GetWorld(), JumpOverLocation, 15, 16, FColor::White, false, 7);
	}

	return true;

}

void UGA_Vault::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
}

void UGA_Vault::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
}

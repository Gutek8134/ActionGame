// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstances/AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "ActionGameTypes.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendspace() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	if (ActionGameCharacter) {
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset) {
			return Data.CharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.MovementBlendspace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetIdleAnimation() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	if (ActionGameCharacter) {
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset) {
			return Data.CharacterAnimDataAsset->CharacterAnimationData.IdleAnimationSequence;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.IdleAnimationSequence : nullptr;
}

UBlendSpace* UAG_AnimInstance::GetCrouchLocomotionBlendspace() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	if (ActionGameCharacter) {
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset) {
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchMovementBlendspace : nullptr;
}

UAnimSequenceBase* UAG_AnimInstance::GetCrouchIdleAnimation() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	if (ActionGameCharacter) {
		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset) {
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationSequence;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationSequence : nullptr;
}

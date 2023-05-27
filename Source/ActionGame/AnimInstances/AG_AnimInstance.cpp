// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionGameCharacter.h"
#include "ActionGameTypes.h"
#include "DataAssets/CharacterAnimDataAsset.h"
#include "AnimInstances/AG_AnimInstance.h"

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

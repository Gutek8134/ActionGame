// Fill out your copyright notice in the Description page of Project Settings.

#include "AnimInstances/AG_AnimInstance.h"
#include "ActionGameCharacter.h"
#include "ActionGameTypes.h"
#include "Inventory/InventoryItemInstance.h"
#include "ActorComponents/InventoryComponent.h"
#include "DataAssets/CharacterAnimDataAsset.h"

UBlendSpace* UAG_AnimInstance::GetLocomotionBlendspace() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	if (ActionGameCharacter) {
		if (const UStaticItemData* ItemData = GetEquippedItemData()) {
			if (ItemData->CharacterAnimationData.MovementBlendspace)
				return ItemData->CharacterAnimationData.MovementBlendspace;
		}



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
		if (const UStaticItemData* ItemData = GetEquippedItemData()) {
			if (ItemData->CharacterAnimationData.IdleAnimationSequence)
				return ItemData->CharacterAnimationData.IdleAnimationSequence;
		}

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
		if (const UStaticItemData* ItemData = GetEquippedItemData()) {
			if (ItemData->CharacterAnimationData.CrouchMovementBlendspace)
				return ItemData->CharacterAnimationData.CrouchMovementBlendspace;
		}

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
		if (const UStaticItemData* ItemData = GetEquippedItemData()) {
			if (ItemData->CharacterAnimationData.CrouchIdleAnimationSequence)
				return ItemData->CharacterAnimationData.CrouchIdleAnimationSequence;
		}

		FCharacterData Data = ActionGameCharacter->GetCharacterData();
		if (Data.CharacterAnimDataAsset) {
			return Data.CharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationSequence;
		}
	}
	return DefaultCharacterAnimDataAsset ? DefaultCharacterAnimDataAsset->CharacterAnimationData.CrouchIdleAnimationSequence : nullptr;
}

const UStaticItemData* UAG_AnimInstance::GetEquippedItemData() const
{
	AActionGameCharacter* ActionGameCharacter = Cast<AActionGameCharacter>(GetOwningActor());
	UInventoryComponent* InventoryComponent = ActionGameCharacter ? ActionGameCharacter->GetInventoryComponent() : nullptr;
	UInventoryItemInstance* ItemInstance = InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
	return ItemInstance ? ItemInstance->GetItemStaticData() : nullptr;
}

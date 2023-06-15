// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_ItemAbility.h"

#include "Inventory/InventoryItemInstance.h"
#include "Inventory/ItemActors/WeaponActor.h"
#include "ActorComponents/InventoryComponent.h"

void UGA_ItemAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) {
	Super::OnGiveAbility(ActorInfo, Spec);

	InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent* UGA_ItemAbility::GetInventoryComponent() const
{
	return InventoryComponent;
}

UInventoryItemInstance* UGA_ItemAbility::GetEquippedItemInstance() const
{
	return InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
}

const UStaticItemData* UGA_ItemAbility::GetEquippedItemStaticData() const
{
	auto EquippedItem = GetEquippedItemInstance();
	return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
}

const UStaticWeaponData* UGA_ItemAbility::GetEquippedWeaponStaticData() const
{
	return Cast<UStaticWeaponData>(GetEquippedItemStaticData());
}

AActor* UGA_ItemAbility::GetEquippedItemActor() const
{
	auto EquippedItem = GetEquippedItemInstance();
	return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}

AWeaponActor* UGA_ItemAbility::GetEquippedWeaponItemActor() const
{
	return Cast<AWeaponActor>(GetEquippedItemActor());
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AG_GameplayAbility.h"
#include "GA_ItemAbility.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UStaticItemData;
class UStaticWeaponData;
class AItemActor;
class AWeaponActor;

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UGA_ItemAbility : public UAG_GameplayAbility
{
	GENERATED_BODY()
	
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		UInventoryItemInstance* GetEquippedItemInstance() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		const UStaticItemData* GetEquippedItemStaticData() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
		const UStaticWeaponData* GetEquippedWeaponStaticData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		AWeaponActor* GetEquippedWeaponItemActor() const;

protected:
	UPROPERTY()
		UInventoryComponent* InventoryComponent = nullptr;
};

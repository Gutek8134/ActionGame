// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionGameTypes.h"
#include "Actors/ItemActor.h"
#include "InventoryItemInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ACTIONGAME_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual void Init(TSubclassOf<UStaticItemData> InItemDataClass);

	FORCEINLINE virtual bool IsSupportedForNetworking() const override { return true; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UStaticItemData* GetItemStaticData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AItemActor* GetItemActor() const;

	UPROPERTY(Replicated)
		TSubclassOf<UStaticItemData> StaticItemDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
		bool bEquipped = false;

	UFUNCTION()
		void OnRep_Equipped();

	virtual void OnEquipped(AActor* InOwner = nullptr);

	virtual void OnUnequipped(AActor* InOwner = nullptr);

	virtual void OnDropped(AActor* InOwner = nullptr);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	UPROPERTY(Replicated)
		AItemActor* ItemActor = nullptr;

	void TryGrantAbilities(AActor* InOwner);

	void TryRemoveAbilities(AActor* InOwner);

	UPROPERTY()
		TArray<FGameplayAbilitySpecHandle> GrantedAbilityHandles;
};


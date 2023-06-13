// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/ItemActor.h"
#include "GameplayTagsManager.h"

#include "InventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void InitializeComponent() override;

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintCallable)
		void AddItem(TSubclassOf<UStaticItemData> InStaticItemDataClass);
	
	UFUNCTION(BlueprintCallable)
		void AddItemInstance(UInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable)
		void RemoveItem(TSubclassOf<UStaticItemData> InStaticItemDataClass);

	UFUNCTION(BlueprintCallable)
		void EquipItem(TSubclassOf<UStaticItemData> InStaticItemDataClass);
	
	UFUNCTION(BlueprintCallable)
		void EquipItemInstance(UInventoryItemInstance* InItemInstance);
	
	UFUNCTION(BlueprintCallable)
		void UnequipItem();

	UFUNCTION(BlueprintCallable)
		void DropItem();

	UFUNCTION(BlueprintCallable)
		void EquipNextItem();

	virtual void GameplayEventCallback(const FGameplayEventData* Payload);

	static FGameplayTag EquipItemActorTag;
	static FGameplayTag DropItemTag;
	static FGameplayTag EquipNextTag;
	static FGameplayTag UnequipTag;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE UInventoryItemInstance* GetEquippedItem() const { return CurrentItem; }

protected:
	UPROPERTY(Replicated)
		FInventoryList InventoryList;

	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<UStaticItemData>> DefaultItems;

	UPROPERTY(Replicated)
		UInventoryItemInstance* CurrentItem = nullptr;

	UFUNCTION()
		void AddInventoryTags();

	void HandleGameplayEventInternal(FGameplayEventData Payload);

	UFUNCTION(Server, Reliable)
		void ServerHandleGameplayEvent(FGameplayEventData Payload);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

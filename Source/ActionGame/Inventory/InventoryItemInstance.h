// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionGameTypes.h"
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

	UFUNCTION(BlueprintCallable)
	const UStaticItemData* GetItemStaticData() const;

	UPROPERTY(Replicated)
		TSubclassOf<UStaticItemData> StaticItemDataClass;

	UPROPERTY(ReplicatedUsing = OnRep_Equipped)
		bool bEquipped = false;

	UFUNCTION()
		void OnRep_Equipped();

	FORCEINLINE virtual void OnEquipped() {}

	FORCEINLINE virtual void OnUnequipped() {}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};


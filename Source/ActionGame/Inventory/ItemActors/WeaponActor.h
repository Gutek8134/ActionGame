// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemActor.h"
#include "WeaponActor.generated.h"

/**
 *
 */
UCLASS()
class ACTIONGAME_API AWeaponActor : public AItemActor
{
	GENERATED_BODY()

public:
	AWeaponActor();

	const UStaticWeaponData* GetStaticWeaponData() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable)
		void PlayWeaponEffects(const FHitResult& InHitResult);

protected:
	UPROPERTY()
		UMeshComponent* MeshComponent = nullptr;

	virtual void InitInternal() override;

	UFUNCTION(NetMulticast, Reliable)
		void MulticastPlayWeaponEffects(const FHitResult& InHitResult);

	void InternalPlayWeaponEffects(const FHitResult& InHitResult);
};

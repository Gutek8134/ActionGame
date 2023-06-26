// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ActionGameTypes.h"
#include "ActionGameStatics.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONGAME_API UActionGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static const UStaticItemData* GetItemStaticData(TSubclassOf<UStaticItemData> ItemDataClass);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ApplyRadialDamage(UObject* WorldContextObject, class AActor* DamageCauser, FVector Location, float Radius, float DamageAmount, TArray<TSubclassOf<class UGameplayEffect>> DamageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType);
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AProjectile* LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UStaticProjectileData> ProjectileDataClass, FTransform Transform, AActor* Owner, APawn* Instigator);
};

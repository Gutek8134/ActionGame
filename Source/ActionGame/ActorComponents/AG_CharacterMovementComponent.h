// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionGameTypes.h"
#include "GameplayTagContainer.h"
#include "AG_CharacterMovementComponent.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class ACTIONGAME_API UAG_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	bool TryTraversal(UAbilitySystemComponent* ASC);

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE EMovementDirectionType GetMovementDirectionType() const { return MovementDirectionType; }
	
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetMovementDirectionType(EMovementDirectionType InMovementDirectionType) { MovementDirectionType = InMovementDirectionType; HandleMovementDirection(); }

	UFUNCTION()
		void OnEnforcedStrafeTagChanged(const FGameplayTag CallvackTag, int32 NewCount);

protected:
	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<UGameplayAbility>> TraversalAbilitiesOrdered;

	UPROPERTY(EditAnywhere)
		EMovementDirectionType MovementDirectionType;

	void HandleMovementDirection();
};

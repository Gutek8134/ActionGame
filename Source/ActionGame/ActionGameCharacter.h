// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "ActionGameTypes.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbility.h"
#include "ActionGameCharacter.generated.h"

class UAG_AbilitySystemComponentBase;
class UAG_AttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

UCLASS(config = Game)
class AActionGameCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CrouchAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

public:
	AActionGameCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	UPROPERTY(EditDefaultsOnly)
		UAG_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
		UAG_AttributeSetBase* AttributeSet;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for jumping input */
	void JumpActionStarted(const FInputActionValue& Value);

	void JumpActionStopped(const FInputActionValue& Value);

	virtual void Landed(const FHitResult& Hit) override;

	/** Called for crouch input */
	void CrouchActionStarted(const FInputActionValue& Value);

	void CrouchActionStopped(const FInputActionValue& Value);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/** Called for crouch input */
	void SprintActionStarted(const FInputActionValue& Value);

	void SprintActionStopped(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION(BLueprintCallable)
		FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
		void SetCharacterData(const FCharacterData& InCharacterData);


	FORCEINLINE class UFootstepsComponent* GetFootstepsComponent() const { return FootstepsComponent; }

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CharacterData)
		FCharacterData CharacterData;

	UFUNCTION()
		void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditDefaultsOnly)
		class UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
		class UFootstepsComponent* FootstepsComponent;


// Gameplay Events
protected:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag JumpEventTag;


// Gameplay Tags
protected:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer InAirTags;

	UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer CrouchTags;
	
	UPROPERTY(EditDefaultsOnly)
		FGameplayTagContainer SprintTags;


// Gameplay Effects
protected:
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> CrouchStateEffect;

//Delegates
protected:
	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;

};


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
class UInventoryComponent;

class UGameplayEffect;
class UGameplayAbility;

class UAG_MotionWarpingComponent;
class UAG_CharacterMovementComponent;

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
	
	//Inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DropItemAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* EquipNextAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* UnequipAction;
	
	//Attacking with items
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AttackAction;

public:
	AActionGameCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FORCEINLINE UAG_MotionWarpingComponent* GetAGMotionWarpingComponent() const { return AGMotionWarpingComponent; }

	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }


protected:

	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;


	UPROPERTY(EditDefaultsOnly)
		UAG_AbilitySystemComponentBase* AbilitySystemComponent;

	UPROPERTY(Transient)
		UAG_AttributeSetBase* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
		UAG_MotionWarpingComponent* AGMotionWarpingComponent;

	UAG_CharacterMovementComponent* AGCharacterMovementComponent;

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

	/** Called for sprint input */
	void SprintActionStarted(const FInputActionValue& Value);

	void SprintActionStopped(const FInputActionValue& Value);

	/** Called for inventory related input*/

	void DropItem(const FInputActionValue& Value);

	void EquipNextItem(const FInputActionValue& Value);
	
	void UnequipItem(const FInputActionValue& Value);

	/** Called for attacking with items*/
	void AttackStarted(const FInputActionValue& Value);

	void AttackEnded(const FInputActionValue& Value);

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
	UFUNCTION(BlueprintCallable)
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

	UPROPERTY(EditAnywhere, Replicated)
		class UInventoryComponent* InventoryComponent;

// Gameplay Events
protected:
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag JumpEventTag;
	
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AttackStartedEventTag;
	
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AttackEndedEventTag;


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


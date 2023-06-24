// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

class AItemActor;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FCharacterData {
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
		TArray<TSubclassOf<class UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
		TArray<TSubclassOf<class UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData {
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly)
		class UBlendSpace* MovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UAnimSequenceBase* IdleAnimationSequence = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UBlendSpace* CrouchMovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
		class UAnimSequenceBase* CrouchIdleAnimationSequence = nullptr;
};

UENUM(BlueprintType)
enum class EFoot : uint8 {
	Left UMETA(Displayname = "Left"),
	Right UMETA(Displayname = "Right"),
};

UCLASS(BlueprintType, Blueprintable)
class UStaticItemData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName AttachmentSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bCanBeEquipped = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FCharacterAnimationData CharacterAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;
};

UCLASS(BlueprintType, Blueprintable)
class UStaticWeaponData : public UStaticItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* AttackSound;
};

UCLASS(BlueprintType, Blueprintable)
class UStaticProjectileData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float DamageRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float GravityMultiplier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float InitialSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		UNiagaraSystem* OnStopVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* OnStopSFX = nullptr;


};

UENUM(BlueprintType)
enum EItemState {
	None UMETA(Displayname = "None"),
	Equipped UMETA(Displayname = "Equipped"),
	Dropped UMETA(Displayname = "Dropped"),
};

UENUM(BlueprintType)
enum class EMovementDirectionType : uint8 {
	None UMETA(DisplayName = "None"),
	OrientToMovement UMETA(DisplayName = "Orient To Movement"),
	Strafe UMETA(DisplayName = "Strafe"),
};
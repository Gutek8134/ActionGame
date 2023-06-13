// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ActionGameTypes.h"
#include "ItemActor.generated.h"

class UInventoryItemInstance;

UCLASS()
class ACTIONGAME_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnEquipped();

	virtual void OnUnequipped();

	virtual void OnDropped();

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Init(UInventoryItemInstance* InInstance);

protected:
	UPROPERTY(Replicated)
		UInventoryItemInstance* ItemInstance = nullptr;
	
	UPROPERTY(ReplicatedUsing = OnRep_ItemState)
		TEnumAsByte<EItemState> ItemState = EItemState::None;

	UFUNCTION()
		void OnRep_ItemState();

	UPROPERTY(EditDefaultsOnly)
		USphereComponent* SphereComponent = nullptr;

	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SpeepResult);


	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UStaticItemData> StaticItemDataClass;
};

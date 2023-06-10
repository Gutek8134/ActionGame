// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"
#include "Inventory/InventoryItemInstance.h"
#include "Engine/ActorChannel.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemActor::OnEquipped()
{
}

void AItemActor::OnUnequipped()
{
}

void AItemActor::OnDropped()
{
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	WroteSomething = Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags) || WroteSomething;

	return WroteSomething;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, ItemInstance);
}

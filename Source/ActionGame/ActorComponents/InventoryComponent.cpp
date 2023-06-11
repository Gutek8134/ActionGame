// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/InventoryComponent.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

#include "Inventory/InventoryList.h"
#include "Inventory/InventoryItemInstance.h"


static TAutoConsoleVariable<int32> CVarShowInventory(
	TEXT("ShowDebugInventory"),
	0,
	TEXT("Shows debug info about inventory")
	TEXT("  0: off/n")
	TEXT("  1: on/n"),
	ECVF_Cheat
);

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	// ...
}


void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	for (const auto& ItemClass : DefaultItems) {
		InventoryList.AddItem(ItemClass);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (const auto& Item : InventoryList.GetItemsRef()) {
		UInventoryItemInstance* ItemInstance = Item.ItemInstance;

		if (IsValid(ItemInstance)) {
			WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UInventoryComponent::AddItem(TSubclassOf<UStaticItemData> InItemStaticDataClass)
{
	InventoryList.AddItem(InItemStaticDataClass);

}

void UInventoryComponent::RemoveItem(TSubclassOf<UStaticItemData> InItemStaticDataClass)
{
	InventoryList.RemoveItem(InItemStaticDataClass);
}

void UInventoryComponent::EquipItem(TSubclassOf<UStaticItemData> InStaticItemDataClass)
{
	if (GetOwner()->HasAuthority()) {
		//UE_LOG(LogTemp, Warning, TEXT("Loking for item class"));

		for (const auto& Item : InventoryList.GetItemsRef()) {
			if (Item.ItemInstance->StaticItemDataClass == InStaticItemDataClass) {
				//UE_LOG(LogTemp, Warning, TEXT("Found!"));
				Item.ItemInstance->OnEquipped(GetOwner());

				CurrentItem = Item.ItemInstance;

				break;
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No authority for equipping"))
	}
}

void UInventoryComponent::UnequipItem()
{
	if (GetOwner()->HasAuthority()) {
		if (IsValid(CurrentItem)) {
			CurrentItem->OnUnequipped();
			CurrentItem = nullptr;
		}
	}
}

void UInventoryComponent::DropItem()
{
	if (GetOwner()->HasAuthority()) {
		if (IsValid(CurrentItem)) {
			CurrentItem->OnDropped();
			RemoveItem(CurrentItem->StaticItemDataClass);
			CurrentItem = nullptr;
		}
	}
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const bool bShowDebug = CVarShowInventory.GetValueOnGameThread() > 0;
	if (bShowDebug) {
		for (const auto& Item : InventoryList.GetItemsRef()) {
			UInventoryItemInstance* ItemInstance = Item.ItemInstance;
			const UStaticItemData* ItemStaticData = ItemInstance->GetItemStaticData();

			if (IsValid(ItemInstance) && IsValid(ItemStaticData)) {
				GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, FString::Printf(TEXT("%s"), *ItemStaticData->Name.ToString()));

			}
		}
	}
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, InventoryList);
	DOREPLIFETIME(UInventoryComponent, CurrentItem);
}
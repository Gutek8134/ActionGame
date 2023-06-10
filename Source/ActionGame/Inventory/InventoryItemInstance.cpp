// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "ActionGameStatics.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UStaticItemData> InItemDataClass) {
	StaticItemDataClass = InItemDataClass;
}

const UStaticItemData* UInventoryItemInstance::GetItemStaticData() const
{
	return UActionGameStatics::GetItemStaticData(StaticItemDataClass);
}

void UInventoryItemInstance::OnRep_Equipped()
{
}

void UInventoryItemInstance::OnEquipped(AActor* InOwner)
{
	if (UWorld* World = GetWorld()) {
		const UStaticItemData* StaticData = GetItemStaticData();

		FTransform Transform;

		ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass, Transform, InOwner);
		ItemActor->Init(this);
		ACharacter* Character = Cast<ACharacter>(InOwner);
		USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr;
		if (SkeletalMesh) {
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
		}
		ItemActor->FinishSpawning(Transform);
	}

}

void UInventoryItemInstance::OnUnequipped()
{
	if (IsValid(ItemActor)) {
		ItemActor->Destroy();
		ItemActor = nullptr;
	}
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, StaticItemDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}
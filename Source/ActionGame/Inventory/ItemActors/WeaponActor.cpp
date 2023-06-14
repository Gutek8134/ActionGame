// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponActor.h"
#include "Inventory/InventoryItemInstance.h"

AWeaponActor::AWeaponActor() {

}

const UStaticWeaponData* AWeaponActor::GetStaticWeaponData() const
{
	return ItemInstance ? Cast<UStaticWeaponData>(ItemInstance->GetItemStaticData()) : nullptr;
}

void AWeaponActor::InitInternal()
{
	Super::InitInternal();

	if (const UStaticWeaponData* WeaponData = GetStaticWeaponData()) {
		if (WeaponData->SkeletalMesh) {
			USkeletalMeshComponent* SkeletalComp = NewObject<USkeletalMeshComponent>(this, USkeletalMeshComponent::StaticClass(), TEXT("Mesh Component"));
			if (SkeletalComp) {
				SkeletalComp->RegisterComponent();
				SkeletalComp->SetSkeletalMesh(WeaponData->SkeletalMesh);
				SkeletalComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				MeshComponent = SkeletalComp;
			}
		}
		else if (WeaponData->StaticMesh) {
			UStaticMeshComponent* StaticComp = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("Mesh Component"));
			if (StaticComp) {
				StaticComp->RegisterComponent();
				StaticComp->SetStaticMesh(WeaponData->StaticMesh);
				StaticComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}
	}
}

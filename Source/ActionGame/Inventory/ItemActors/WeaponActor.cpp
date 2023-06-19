// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponActor.h"
#include "Inventory/InventoryItemInstance.h"
#include "PhysicalMaterials/AG_PhysicalMaterial.h"
#include "ActionGameTypes.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AWeaponActor::AWeaponActor() {

}

const UStaticWeaponData* AWeaponActor::GetStaticWeaponData() const
{
	return ItemInstance ? Cast<UStaticWeaponData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponActor::GetMuzzleLocation() const
{
	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation();
}

void AWeaponActor::PlayWeaponEffects(const FHitResult& InHitResult)
{
	if (!HasAuthority()) {
		MulticastPlayWeaponEffects(InHitResult);
	}
	else
	{
		InternalPlayWeaponEffects(InHitResult);
	}
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

void AWeaponActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if (!Owner || Owner->GetLocalRole() != ROLE_Authority)
		InternalPlayWeaponEffects(InHitResult);


}

void AWeaponActor::InternalPlayWeaponEffects(const FHitResult& InHitResult)
{
	if (InHitResult.PhysMaterial.IsValid()) {
		UAG_PhysicalMaterial* PhysicalMaterial = Cast<UAG_PhysicalMaterial>(InHitResult.PhysMaterial.Get());
		if (PhysicalMaterial) {
			if (PhysicalMaterial->PointImpactSound) {
				UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->PointImpactSound, InHitResult.ImpactPoint, 1.f);
			}

			if (PhysicalMaterial->PointImpactVFX) {
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactVFX, InHitResult.ImpactPoint);
			}
		}


	}
	if (const UStaticWeaponData* WeaponData = GetStaticWeaponData()) {
		UGameplayStatics::PlaySoundAtLocation(this, WeaponData->AttackSound, GetActorLocation(), 100.f);
	}
}

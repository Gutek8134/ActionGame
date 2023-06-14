// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "ActionGameStatics.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
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
	if (UWorld* World = InOwner->GetWorld()) {
		const UStaticItemData* StaticData = GetItemStaticData();

		FTransform Transform;

		ItemActor = World->SpawnActorDeferred<AItemActor>(GetItemStaticData()->ItemActorClass, Transform, InOwner);
		ItemActor->Init(this);
		ItemActor->OnEquipped();
		ItemActor->FinishSpawning(Transform);
		ACharacter* Character = Cast<ACharacter>(InOwner);
		USkeletalMeshComponent* SkeletalMesh = Character ? Character->GetMesh() : nullptr;
		//UE_LOG(LogTemp, Warning, L"Attaching to socket %s", *Character->GetHumanReadableName());

		if (SkeletalMesh) {
			ItemActor->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, StaticData->AttachmentSocket);
			//UE_LOG(LogTemp, Warning, L"Attaching to socket %s", *StaticData->AttachmentSocket.ToString());

		}
	}
	else {
		//UE_LOG(LogTemp, Warning, L"WTH? No world!");

	}

	TryGrantAbilities(InOwner);
	bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* InOwner)
{
	if (IsValid(ItemActor)) {
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	TryRemoveAbilities(InOwner);
	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if (ItemActor) {
		ItemActor->OnDropped();
	}

	TryRemoveAbilities(InOwner);
	bEquipped = false;
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, StaticItemDataClass);
	DOREPLIFETIME(UInventoryItemInstance, bEquipped);
	DOREPLIFETIME(UInventoryItemInstance, ItemActor);
}

void UInventoryItemInstance::TryGrantAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority()) {
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {
			const UStaticItemData* StaticData = GetItemStaticData();

			for (const auto& ItemAbility : StaticData->GrantedAbilities) {
				GrantedAbilityHandles.Add(AbilityComponent->GiveAbility(FGameplayAbilitySpec(ItemAbility)));
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveAbilities(AActor* InOwner)
{
	if (InOwner && InOwner->HasAuthority()) {
		if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {
			const UStaticItemData* StaticData = GetItemStaticData();

			for (const auto& ItemAbility : GrantedAbilityHandles) {
				AbilityComponent->ClearAbility(ItemAbility);
			}

			GrantedAbilityHandles.Empty();
		}
	}
}

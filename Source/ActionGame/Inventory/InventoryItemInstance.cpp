// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemInstance.h"
#include "ActionGameStatics.h"
#include "GameFramework/Character.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemLog.h"

#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Init(TSubclassOf<UStaticItemData> InItemDataClass) {
	StaticItemDataClass = InItemDataClass;
}

const UStaticItemData* UInventoryItemInstance::GetItemStaticData() const
{
	return UActionGameStatics::GetItemStaticData(StaticItemDataClass);
}

AItemActor* UInventoryItemInstance::GetItemActor() const
{
	return ItemActor;
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
	TryApplyEffects(InOwner);
	bEquipped = true;
}

void UInventoryItemInstance::OnUnequipped(AActor* InOwner)
{
	if (IsValid(ItemActor)) {
		ItemActor->Destroy();
		ItemActor = nullptr;
	}

	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
	bEquipped = false;
}

void UInventoryItemInstance::OnDropped(AActor* InOwner)
{
	if (ItemActor) {
		ItemActor->OnDropped();
	}

	TryRemoveAbilities(InOwner);
	TryRemoveEffects(InOwner);
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

void UInventoryItemInstance::TryApplyEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner))
	{

		const UStaticItemData* ItemStaticData = GetItemStaticData();
		FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

		for (const auto& GameplayEffect : ItemStaticData->OngoingEffects) {
			if (!GameplayEffect.Get()) continue;

			FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);

			if (SpecHandle.IsValid()) {
				FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied()) {
					ABILITY_LOG(Log, TEXT("Item %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
				}
				else {
					OngoingEffectHandles.Add(ActiveGEHandle);
				}
			}
		}
	}
}

void UInventoryItemInstance::TryRemoveEffects(AActor* InOwner)
{
	if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner)) {
		for (const auto& OngoingEffectHandle : OngoingEffectHandles) {
			if (OngoingEffectHandle.IsValid()) {
				AbilityComponent->RemoveActiveGameplayEffect(OngoingEffectHandle);
			}
		}
	}

	OngoingEffectHandles.Empty();
}

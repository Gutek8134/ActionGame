// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemActor.h"
#include "Inventory/InventoryItemInstance.h"
#include "Engine/ActorChannel.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Net/UnrealNetwork.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnSphereOverlap);
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
}

void AItemActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SpeepResult)
{
	FGameplayEventData EventPayload;
	EventPayload.OptionalObject = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, OverlapEventTag, EventPayload);
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
	ItemState = EItemState::Equipped;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnUnequipped()
{
	ItemState = EItemState::None;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemActor::OnDropped()
{
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	ItemState = EItemState::Dropped;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (AActor* ActorOwner = GetOwner()) {
		const FVector Location = GetActorLocation();
		const FVector ForwardVector = ActorOwner->GetActorForwardVector();

		const float dropItemDist = 100.f;
		const float dropItemTraceDist = 100.f;

		const FVector TraceStart = Location + ForwardVector * dropItemDist;
		const FVector TraceEnd = TraceStart - FVector::UpVector * dropItemTraceDist;

		TArray<AActor*> ActorsToIgnore = { GetOwner() };

		FHitResult TraceHit;

		static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
		const bool bShowInventoryDebug = CVar->GetInt() > 0;

		EDrawDebugTrace::Type DebugDrawType = bShowInventoryDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		if (UKismetSystemLibrary::LineTraceSingleByProfile(this, TraceStart, TraceEnd, TEXT("worldStatic"), true, ActorsToIgnore, DebugDrawType, TraceHit, true)) {
			if (TraceHit.bBlockingHit) {
				SetActorLocation(TraceHit.Location);
				return;
			}
		}
	}
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);

	return WroteSomething;
}

void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, ItemInstance);
	DOREPLIFETIME(AItemActor, ItemState);
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGameCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AttributeSets/AG_AttributeSetBase.h"
#include "AbilitySystem/Components/AG_AbilitySystemComponentBase.h"
#include "GameplayEffectExtension.h"
#include "DataAssets/CharacterDataAsset.h"

#include "ActorComponents/FootstepsComponent.h"
#include "ActorComponents/AG_CharacterMovementComponent.h"
#include "ActorComponents/AG_MotionWarpingComponent.h"
#include "ActorComponents/InventoryComponent.h"

#include "Net/UnrealNetwork.h"

#include "AbilitySystemLog.h"


//////////////////////////////////////////////////////////////////////////
// AActionGameCharacter

AActionGameCharacter::AActionGameCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UAG_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	AGCharacterMovementComponent = Cast<UAG_CharacterMovementComponent>(GetCharacterMovement());

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)


	// Ability System

	AbilitySystemComponent = CreateDefaultSubobject<UAG_AbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UAG_AttributeSetBase>(TEXT("AttributeSet"));

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxMovementSpeedAttribute()).AddUObject(this, &AActionGameCharacter::OnMaxMovementSpeedChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &AActionGameCharacter::OnHealthAttributeChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(L"State.Ragdoll"), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AActionGameCharacter::OnRagdollTagChanged);

	FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>(TEXT("Footsteps Component"));
	
	AGMotionWarpingComponent = CreateDefaultSubobject<UAG_MotionWarpingComponent>(TEXT("Motion Warping Component"));
	
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->SetIsReplicated(true);
}

void AActionGameCharacter::PostLoad()
{
	Super::PostLoad();

	if (IsValid(CharacterDataAsset)) {
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
}

bool AActionGameCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid()) {
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}

UAbilitySystemComponent* AActionGameCharacter::GetAbilitySystemComponent() const {
	return AbilitySystemComponent;
}

void AActionGameCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent) {
		for (auto DefaultAbility : CharacterData.Abilities) {
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AActionGameCharacter::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects) {
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
		}

	}
}

void AActionGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
	ApplyStartupEffects();
}

void AActionGameCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AActionGameCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

FCharacterData AActionGameCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AActionGameCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(InCharacterData);
}

void AActionGameCharacter::OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data)
{
	GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
}

void AActionGameCharacter::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0 && Data.OldValue > 0) {
		AActionGameCharacter* OtherCharacter = nullptr;
		if (Data.GEModData) {
			const FGameplayEffectContextHandle& EffectContext = Data.GEModData->EffectSpec.GetEffectContext();
			OtherCharacter = Cast<AActionGameCharacter>(EffectContext.GetInstigator());
		}

		FGameplayEventData EventPayload;
		EventPayload.EventTag = ZeroHealthEventTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, ZeroHealthEventTag, EventPayload);
	}
}

void AActionGameCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AActionGameCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AActionGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AActionGameCharacter, CharacterData);
	DOREPLIFETIME(AActionGameCharacter, InventoryComponent);
}

void AActionGameCharacter::StartRagdoll()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	if (SkeletalMesh && !SkeletalMesh->IsSimulatingPhysics()) {
		SkeletalMesh->SetCollisionProfileName(L"Ragdoll");
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		SkeletalMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		SkeletalMesh->WakeAllRigidBodies();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AActionGameCharacter::OnRagdollTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount > 0) {
		StartRagdoll();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AActionGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::JumpActionStarted);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Look);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AActionGameCharacter::CrouchActionStarted);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed , this, &AActionGameCharacter::CrouchActionStopped);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AActionGameCharacter::SprintActionStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed , this, &AActionGameCharacter::SprintActionStopped);

		//Inventory
		EnhancedInputComponent->BindAction(DropItemAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::DropItem);
		EnhancedInputComponent->BindAction(EquipNextAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::EquipNextItem);
		EnhancedInputComponent->BindAction(UnequipAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::UnequipItem);

		//Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AActionGameCharacter::AttackStarted);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &AActionGameCharacter::AttackEnded);
		
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AActionGameCharacter::AimStarted);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AActionGameCharacter::AimEnded);
	}

}

void AActionGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AActionGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AActionGameCharacter::JumpActionStarted(const FInputActionValue& Value)
{
	AGCharacterMovementComponent->TryTraversal(AbilitySystemComponent);
}

void AActionGameCharacter::JumpActionStopped(const FInputActionValue& Value)
{
	//ACharacter::StopJumping();
}

void AActionGameCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (AbilitySystemComponent) {
		AbilitySystemComponent->RemoveActiveEffectsWithTags(InAirTags);
	}
}

void AActionGameCharacter::CrouchActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->TryActivateAbilitiesByTag(CrouchTags, true);
	}
}

void AActionGameCharacter::CrouchActionStopped(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->CancelAbilities(&CrouchTags);
	}
}

void AActionGameCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (!CrouchStateEffect.Get()) return;

	if (AbilitySystemComponent) {
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(CrouchStateEffect, 1, EffectContext);

		if (SpecHandle.IsValid()) {
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

			if (!ActiveGEHandle.WasSuccessfullyApplied()) {
				ABILITY_LOG(Log, TEXT("Ability %s failed to apply crouch effect %s"), *GetName(), *GetNameSafe(CrouchStateEffect));
			}
		}
	}
}

void AActionGameCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(CrouchStateEffect, AbilitySystemComponent);
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AActionGameCharacter::SprintActionStarted(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->TryActivateAbilitiesByTag(SprintTags, true);
	}
}

void AActionGameCharacter::SprintActionStopped(const FInputActionValue& Value)
{
	if (AbilitySystemComponent) {
		AbilitySystemComponent->CancelAbilities(&SprintTags);
	}
}

void AActionGameCharacter::DropItem(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::DropItemTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventPayload);
}

void AActionGameCharacter::EquipNextItem(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::EquipNextTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextTag, EventPayload);
}

void AActionGameCharacter::UnequipItem(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::UnequipTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnequipTag, EventPayload);
}

void AActionGameCharacter::AttackStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackStartedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackStartedEventTag, EventPayload);
}

void AActionGameCharacter::AttackEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AttackEndedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AttackEndedEventTag, EventPayload);
}

void AActionGameCharacter::AimStarted(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AimStartedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimStartedEventTag, EventPayload);
}

void AActionGameCharacter::AimEnded(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = AimEndedEventTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AimEndedEventTag, EventPayload);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameStatics.h"

#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/Projectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemCOmponent.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
	TEXT("ShowDebugRadialDamage"),
	0,
	TEXT("Draws debug info about radial damage")
	TEXT("	0: off/n")
	TEXT("	1: on/n"),
	ECVF_Cheat
);

const UStaticItemData* UActionGameStatics::GetItemStaticData(TSubclassOf<UStaticItemData> ItemDataClass) {
	if (IsValid(ItemDataClass))
		return GetDefault<UStaticItemData>(ItemDataClass);

	return nullptr;
}

void UActionGameStatics::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius, float DamageAmount, TArray<TSubclassOf<class UGameplayEffect>> DamageEffects, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType)
{
	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore = { DamageCauser };

	UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

	const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());

	for (AActor* Actor : OutActors) {
		FHitResult HitResult;
		if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true)) {
			AActor* Target = HitResult.GetActor();

			if (Target == Actor) {
				bool bWasApplied = false;

				if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target)) {
					auto EffectContext = AbilityComponent->MakeEffectContext();
					EffectContext.AddInstigator(DamageCauser, DamageCauser);

					for (const auto& Effect : DamageEffects) {
						auto SpecHandle = AbilityComponent->MakeOutgoingSpec(Effect, 1, EffectContext);

						if (SpecHandle.IsValid()) {
							UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(L"Attribute.Health"), -DamageAmount);

							FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

							if (ActiveGEHandle.WasSuccessfullyApplied()) {
								bWasApplied = true;
							}
						}
					}
				}

				if (bDebug) {
					DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 14, bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::White, 0, false, 1.f);
				}
			}
			else if (bDebug) {
				DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
				DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 14, FColor::Red, false, 4.f, 0, 1);
				DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::White, 0, false, 1.f);
			}
		}
		else if (bDebug) {
			DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
			DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 14, FColor::Red, false, 4.f, 0, 1);
			DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()), nullptr, FColor::White, 0, false, 1.f);
		}
	}
	if (bDebug) {	
		DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16, FColor::White, false, 4.f, 0, 1);
	}
}

AProjectile* UActionGameStatics::LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UStaticProjectileData> ProjectileDataClass, FTransform Transform, AActor* Owner, AActor* Instigator)
{
	return nullptr;
}

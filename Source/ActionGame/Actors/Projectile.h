// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionGameTypes.h"
#include "Projectile.generated.h"

UCLASS()
class ACTIONGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	const UStaticProjectileData* GetProjectileData() const;

	UPROPERTY(BlueprintReadOnly, Replicated)
		TSubclassOf<UStaticProjectileData> ProjectileDataClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditDefaultsOnly)
		class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	void DebugDrawPath() const;

	UPROPERTY()
		class UStaticMeshComponent* StaticMeshComponent = nullptr;

	UFUNCTION()
		void OnProjectileStop(const FHitResult& ImpactResult);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

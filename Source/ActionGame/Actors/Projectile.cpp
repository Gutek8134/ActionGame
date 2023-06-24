// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "ActionGameStatics.h"

#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjectiles(
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("Draws debug info about projectiles")
	TEXT("	0: off/n")
	TEXT("	1: on/n"),
	ECVF_Cheat
);

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(L"Projectile Movement Component");
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(L"Mesh Component");
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName(L"Projectile");
	StaticMeshComponent->SetReceivesDecals(false);
}

const UStaticProjectileData* AProjectile::GetProjectileData() const
{
	if (IsValid(ProjectileDataClass))
		return GetDefault<UStaticProjectileData>(ProjectileDataClass);
	return nullptr;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	const UStaticProjectileData* ProjectileData = GetProjectileData();
	if (ProjectileData && ProjectileMovementComponent) {
		if (ProjectileData->StaticMesh) {
			StaticMeshComponent->SetStaticMesh(ProjectileData->StaticMesh);
		}
		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileData->MaxSpeed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileData->GravityMultiplier;

		ProjectileMovementComponent->Velocity = ProjectileData->InitialSpeed * GetActorForwardVector();
	}

	auto DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();
	if (DebugShowProjectile > 0) {
		DebugDrawPath();
	}
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UStaticProjectileData* ProjectileData = GetProjectileData();
	if (ProjectileData) {
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileData->OnStopSFX, GetActorLocation(), 1.f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileData->OnStopVFX, GetActorLocation());
	}

	Super::EndPlay(EndPlayReason);
}

void AProjectile::DebugDrawPath() const
{
	const UStaticProjectileData* ProjectileData = GetProjectileData();
	if (ProjectileData) {
		FPredictProjectilePathParams PredictionProjectileParams;
		PredictionProjectileParams.StartLocation = GetActorLocation();
		PredictionProjectileParams.LaunchVelocity = ProjectileData->InitialSpeed * GetActorForwardVector();
		PredictionProjectileParams.TraceChannel = ECollisionChannel::ECC_Visibility;
		PredictionProjectileParams.bTraceComplex = true;
		PredictionProjectileParams.bTraceWithCollision = true;
		PredictionProjectileParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictionProjectileParams.DrawDebugTime = 3.f;
		PredictionProjectileParams.OverrideGravityZ = ProjectileData->GravityMultiplier;

		FPredictProjectilePathResult PredictProjectilePathResult;
		if (UGameplayStatics::PredictProjectilePath(this, PredictionProjectileParams, PredictProjectilePathResult)) {
			DrawDebugSphere(GetWorld(), PredictProjectilePathResult.HitResult.Location, 50, 10, FColor::Red);
		}
	}
}

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
 
	const UStaticProjectileData* ProjectileData = GetProjectileData();
	if(ProjectileData){}
	Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AProjectile, ProjectileDataClass);
}

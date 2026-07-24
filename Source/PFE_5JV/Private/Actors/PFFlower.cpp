// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PFFlower.h"
#include "Actors/PoolSubsystem.h"

// Sets default values
APFFlower::APFFlower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	FlowerMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("FlowerMeshComponent");
	SetRootComponent(FlowerMeshComponent);

	// PoolSubsystemPtr_ = GetWorld()->GetSubsystem<UPoolSubsystem>();
}

void APFFlower::BeginPlay()
{
	Super::BeginPlay();
	
	if (!FlowerMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("FlowerMeshComponent is null in BeginPlay!"));
		return;
	}

	UMaterialInterface* MaterialInterface = FlowerMeshComponent->GetMaterial(0);
	if (!MaterialInterface) UE_LOG(LogTemp, Error, TEXT("No material assigned to FlowerMeshComponent in BeginPlay!"));

	DynamicMaterial = FlowerMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (!DynamicMaterial) UE_LOG(LogTemp, Error, TEXT("Failed to create DynamicMaterial in BeginPlay!"));
}

// void APFFlower::OnReturnToPool_Implementation_Implementation()
// {
// 	PoolSubsystemPtr_->ReturnToPool(this);
// }
//
// void APFFlower::OnSpawnFromPool_Implementation_Implementation()
// {
// 	
// }




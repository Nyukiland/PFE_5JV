// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PoolSubsystem.h"
#include "Actors/PFPoolable.h"

void UPoolSubsystem::InitializePool(TSubclassOf<AActor> PoolClass, int Amount)
{
	if(PoolClass.Get()->ImplementsInterface(UPFPoolable::StaticClass()))
	{
		FPoolArrays& ObjectPool = ObjectPools.FindOrAdd(PoolClass);
		for(int i = 0; i < Amount; i++)
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			FVector Location = FVector(0.f, 0.f, -2000.f);
			FRotator Rotation = FRotator(0.f, 0.f, 0.f);
			AActor* Poolable = GetWorld()->SpawnActor<AActor>(PoolClass, Location, Rotation, SpawnParameters);
			ObjectPool.AddToPool(Poolable);
		}
	}
}

// Version blueprint de la fonction templatée
void UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation,
	AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, Location, Rotation);
}

void UPoolSubsystem::ReturnToPool(TSubclassOf<AActor> PoolClass)
{
	if(PoolClass->ImplementsInterface(UPFPoolable::StaticClass()) == false) return;

	FPoolArrays* ObjectPool = ObjectPools.Find(PoolClass);
	if(ObjectPool == nullptr) return;
	if(ObjectPool->PlacedObjectsIsEmpty() == true) return;
	
	for(AActor* PlacedObject : ObjectPool->PlacedObjects)
	{
		IPFPoolable::Execute_OnReturnToPool(PlacedObject);
	}
	ObjectPool->ReturnToPool();
}

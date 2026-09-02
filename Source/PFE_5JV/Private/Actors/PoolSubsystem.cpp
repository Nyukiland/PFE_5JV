// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PoolSubsystem.h"
#include "Actors/PFPoolable.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"

void UPoolSubsystem::InitializePool(TSubclassOf<AActor> PoolClass, int Amount)
{
	if(PoolClass.Get()->ImplementsInterface(UPFPoolable::StaticClass()))
	{
		FPFPoolArrays& ObjectPool = ObjectPools.FindOrAdd(PoolClass);
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
void UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, FVector Scale,
	AActor*& SpawnedActor)
{
	SpawnedActor = SpawnFromPool<AActor>(PoolClass, Location, Rotation, Scale);
}

void UPoolSubsystem::ReturnToPool(TSubclassOf<AActor> PoolClass)
{
	if(PoolClass->ImplementsInterface(UPFPoolable::StaticClass()) == false) return;

	FPFPoolArrays* ObjectPool = ObjectPools.Find(PoolClass);
	if(ObjectPool == nullptr) return;
	if(ObjectPool->ReadyToBeReplacedIsEmpty() == true) return;
	
	for(AActor* ReadyToBeReplaced : ObjectPool->ReadyToBeReplaced)
	{
		IPFPoolable::Execute_OnReturnToPool(ReadyToBeReplaced);
	}
	ObjectPool->ReturnToPool();
}

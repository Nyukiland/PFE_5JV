// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PoolSubsystem.h"
#include "Actors/PFPoolable.h"

AActor* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation)
{
	AActor* PooledActor = nullptr;

	if(PoolClass.Get()->ImplementsInterface(UPFPoolable::StaticClass()))
	{
		if(ObjectPool.IsEmpty())
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			PooledActor = GetWorld()->SpawnActor<AActor>(PoolClass, Location, Rotation, SpawnParameters);
		}
		else
		{
			PooledActor = ObjectPool.Pop();
			PooledActor->SetActorLocationAndRotation(Location, Rotation);
		}

		IPFPoolable::Execute_OnSpawnFromPool(PooledActor);
	}

	return PooledActor;
}

void UPoolSubsystem::ReturnToPool(AActor* Poolable)
{
	if(Poolable->GetClass()->ImplementsInterface(UPFPoolable::StaticClass()))
	{
		IPFPoolable::Execute_OnReturnToPool(Poolable);
		ObjectPool.Add(Poolable);
	}
	else
	{
		Poolable->Destroy();	
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PoolSubsystem.h"
#include "Actors/PFPoolable.h"

void UPoolSubsystem::ImplementInitialPool(TSubclassOf<AActor> PoolClass, int Amount)
{
	for(int i = 0; i < Amount; i++)
	{
		FActorSpawnParameters SpawnParameters;
        SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        FVector Location = FVector(0.f, 0.f, 0.f);
        FRotator Rotation = FRotator(0.f, 0.f, 0.f);
        AActor* Poolable = GetWorld()->SpawnActor<AActor>(PoolClass, Location, Rotation, SpawnParameters);
        ObjectPool.Add(Poolable);
	}
}

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
			PlacedObjects.Add(PooledActor);
			PlacedObjectTransforms.Add(PooledActor->GetTransform());
		}
		else
		{
			PooledActor = ObjectPool.Pop();
			PlacedObjects.Add(PooledActor);
			PooledActor->SetActorLocationAndRotation(Location, Rotation);
			PlacedObjectTransforms.Add(PooledActor->GetTransform());
			// Change la scale
			// Change le materiau du pooled actor
			// A mettre dans le pooled Actor plutôt ?
		}

		IPFPoolable::Execute_OnSpawnFromPool(PooledActor);
	}

	return PooledActor;
}

void UPoolSubsystem::ReturnToPool(TSubclassOf<AActor> PoolClass)
{
	for(AActor* PlacedObject : PlacedObjects)
	{
		if(PlacedObject->GetClass()->ImplementsInterface(UPFPoolable::StaticClass()))
		{
			IPFPoolable::Execute_OnReturnToPool(PlacedObject);
			// Fait apparaitre les HISM pour tous les acteurs dans le tableau
		}
		else
		{
			PlacedObject->Destroy();	
		}
	}

	ObjectPool.Append(PlacedObjects);
	PlacedObjects.Empty();
}

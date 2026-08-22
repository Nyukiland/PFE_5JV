// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPoolable.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"

USTRUCT()
struct FPFPoolArrays
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	UPROPERTY()
	TArray<AActor*> PlacedObjects;

	UPROPERTY()
	TArray<FTransform> PlacedObjectTransforms;

	bool PoolIsEmpty() const {
		return ObjectPool.IsEmpty();
	}

	bool PlacedObjectsIsEmpty() const {
		return PlacedObjects.IsEmpty();
	}

	void AddToPool(AActor* ActorToAdd)
	{
		ObjectPool.Add(ActorToAdd);
	}

	void AddToPlacedObject(AActor* ActorToAdd, const FTransform& Transform)
	{
		PlacedObjects.Add(ActorToAdd);
		PlacedObjectTransforms.Add(Transform);
	}

	void ReturnToPool()
	{
		ObjectPool.Append(PlacedObjects);
		PlacedObjects.Empty();
		PlacedObjectTransforms.Empty();
	}

	AActor* Pop()
	{
		return ObjectPool.Pop();
	}

	int PlacedObjectsNum() const
	{
		return PlacedObjects.Num();
	}
	
};


UCLASS()
class PFE_5JV_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static constexpr int InitialPoolSize = 20;
	
	UPROPERTY()
	TMap<UClass*, FPFPoolArrays> ObjectPools;
		
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void InitializePool(TSubclassOf<AActor> PoolClass, int Amount);
	
	// fonction templatisée qu'on utilisera en C++
	template<typename T>
	T* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation);

	// La version de la fonction utilisable en blueprint : 
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem", meta=(DeterminesOutputType="PoolClass", DynamicOutputType="SpawnedActor"))
	// comme la fonction C++ est templatisée, les meta servent à déterminer quelle classe d'acteur on a donné en paramètre (PoolClass)
	// pour récupérer le bon type d'acteur à la sortie (SpawnedActor)
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, AActor*& SpawnedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(TSubclassOf<AActor> PoolClass);
	
};

template <typename T>
T* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation)
{
	T* PooledActor = nullptr;

	if(PoolClass.Get()->ImplementsInterface(UPFPoolable::StaticClass()))
	{
		FPFPoolArrays& ObjectPool = ObjectPools.FindOrAdd(PoolClass);
		if(ObjectPool.PoolIsEmpty())
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			PooledActor = GetWorld()->SpawnActor<T>(PoolClass, Location, Rotation, SpawnParameters);
			ObjectPool.AddToPlacedObject(PooledActor, PooledActor->GetTransform());
		}
		else
		{
			PooledActor = CastChecked<T>(ObjectPool.Pop());
			PooledActor->SetActorLocationAndRotation(Location, Rotation);
			ObjectPool.AddToPlacedObject(PooledActor, PooledActor->GetTransform());
		}

		IPFPoolable::Execute_OnSpawnFromPool(PooledActor);
	}

	return PooledActor;
}

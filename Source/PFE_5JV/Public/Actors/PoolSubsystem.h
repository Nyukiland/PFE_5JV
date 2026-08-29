// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPoolable.h"
#include "Subsystems/WorldSubsystem.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"
#include "PoolSubsystem.generated.h"

struct FPFPoolArrays;

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
	T* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, FVector Scale);

	// La version de la fonction utilisable en blueprint : 
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem", meta=(DeterminesOutputType="PoolClass", DynamicOutputType="SpawnedActor"))
	// comme la fonction C++ est templatisée, les meta servent à déterminer quelle classe d'acteur on a donné en paramètre (PoolClass)
	// pour récupérer le bon type d'acteur à la sortie (SpawnedActor)
	void SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, FVector Scale, AActor*& SpawnedActor);
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(TSubclassOf<AActor> PoolClass);
	
};

template <typename T>
T* UPoolSubsystem::SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation, FVector Scale)
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
			PooledActor->SetActorScale3D(Scale);
			ObjectPool.AddToPlacedObject(PooledActor, PooledActor->GetTransform());
		}
		else
		{
			PooledActor = CastChecked<T>(ObjectPool.Pop());
			PooledActor->SetActorLocationAndRotation(Location, Rotation);
			PooledActor->SetActorScale3D(Scale);
			if(const UStaticMeshComponent* MeshComp = PooledActor->template FindComponentByClass<UStaticMeshComponent>())
			{
				FTransform Transform = MeshComp->GetComponentTransform();
				ObjectPool.AddToPlacedObject(PooledActor, Transform);
			}
		}

		IPFPoolable::Execute_OnSpawnFromPool(PooledActor);
	}

	return PooledActor;
}

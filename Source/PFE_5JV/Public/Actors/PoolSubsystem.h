// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PFE_5JV_API UPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ImplementInitialPool(TSubclassOf<AActor> PoolClass, int Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	AActor* SpawnFromPool(TSubclassOf<AActor> PoolClass, FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = "Pool Subsystem")
	void ReturnToPool(TSubclassOf<AActor> PoolClass);

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	UPROPERTY()
	TArray<AActor*> PlacedObjects;

	UPROPERTY()
	TArray<FTransform> PlacedObjectTransforms;


};

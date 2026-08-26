#pragma once

#include "Actors/PFFlower.h"
#include "CoreMinimal.h"
#include "PFFlowerSpawnerTypes.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EPFFlowerEnvironment : uint8
{
	EPFFS_Landscape UMETA(DisplayName = "Landscape"),
	EPFFS_Water UMETA(DisplayName = "Water"),
	EPFFS_Cliff UMETA(DisplayName = "Cliff"),
};

UENUM(BlueprintType)
enum class EPFFlowerColor : uint8
{
	EPFFC_None UMETA(DisplayName = "None"),
	EPFFC_Blue UMETA(DisplayName = "Blue"),
	EPFFC_Red UMETA(DisplayName = "Red"),
	EPFFC_Yellow UMETA(DisplayName = "Yellow"),
	EPFFC_Purple UMETA(DisplayName = "Purple"),
};

UENUM(BlueprintType)
enum class EPFCustomDataVersion : uint8
{
	EPFFC_Actor UMETA(DisplayName = "Actor"),
	EPFFC_Hism UMETA(DisplayName = "HISM"),
};

USTRUCT(Blueprintable, BlueprintType)
struct FPFStaticMeshModelData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EPFFlowerEnvironment EnvironmentType = EPFFlowerEnvironment::EPFFS_Landscape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UHierarchicalInstancedStaticMeshComponent* ActiveModelHismPtr_ = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ActiveModelIndex = -1;
};

USTRUCT(Blueprintable, BlueprintType)
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

USTRUCT(Blueprintable, BlueprintType)
struct FPFEnvironmentFlowers
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<TSubclassOf<APFFlower>> FlowerClasses;
	
	bool IsEmpty() const
	{
		return FlowerClasses.IsEmpty();
	}
	
	void AddUnique(const TSubclassOf<APFFlower> FlowerClass)
	{
		FlowerClasses.AddUnique(FlowerClass);
	}
};


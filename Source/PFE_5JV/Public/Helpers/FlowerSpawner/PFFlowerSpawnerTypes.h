#pragma once

#include "CoreMinimal.h"
#include "PFFlowerSpawnerTypes.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EPFVegetationSupportType : uint8
{
	EPFVS_Landscape UMETA(DisplayName = "Landscape"),
	EPFVS_Water UMETA(DisplayName = "Water"),
	EPFVS_Cliff UMETA(DisplayName = "Cliff"),
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
struct FPFHismData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HISM Data")
	UHierarchicalInstancedStaticMeshComponent* HismPtr_ = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HISM Data")
	int Index = -1;
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


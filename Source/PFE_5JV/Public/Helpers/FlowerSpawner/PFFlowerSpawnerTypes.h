#pragma once

#include "Actors/PFFlower.h"
#include "CoreMinimal.h"
#include "PFFlowerSpawnerTypes.generated.h"

class UHierarchicalInstancedStaticMeshComponent;

UENUM(BlueprintType)
enum class EPFFlowerEnvironment : uint8
{
	EPFFS_None UMETA(DisplayName = "None"),
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
	EPFFlowerEnvironment EnvironmentType = EPFFlowerEnvironment::EPFFS_None;

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
	TArray<AActor*> GrowingObjects;
	
	UPROPERTY()
	TArray<AActor*> ReadyToBeReplaced;
	
	UPROPERTY()
	TArray<FTransform> GrowingObjectTransforms;
	
	UPROPERTY()
	TArray<FTransform> ReadyToBeReplacedTransforms;
	
	UPROPERTY()
	TArray<float> GrowthAlphas;
	

	bool PoolIsEmpty() const {
		return ObjectPool.IsEmpty();
	}

	bool ReadyToBeReplacedIsEmpty() const {
		return ReadyToBeReplaced.IsEmpty();
	}

	void AddToPool(AActor* ActorToAdd)
	{
		ObjectPool.Add(ActorToAdd);
	}

	void AddToGrowingObjects(AActor* ActorToAdd, const FTransform& Transform)
	{
		GrowingObjects.Add(ActorToAdd);
		GrowingObjectTransforms.Add(Transform);
		GrowthAlphas.Add(0.f);
	}

	void ReturnToPool()
	{
		ObjectPool.Append(ReadyToBeReplaced);
		ReadyToBeReplaced.Empty();
		ReadyToBeReplacedTransforms.Empty();
	}
	
	void TransferFromGrowingToReadyToBeReplaced(int Index)
	{
		if (
			GrowingObjects.IsValidIndex(Index) && 
			GrowingObjectTransforms.IsValidIndex(Index) &&
			GrowthAlphas.IsValidIndex(Index) 
		){
			AActor* Actor = GrowingObjects[Index];
			FTransform& Transform = GrowingObjectTransforms[Index];
			
			GrowingObjects.RemoveAtSwap(Index);
			GrowingObjectTransforms.RemoveAtSwap(Index);
			GrowthAlphas.RemoveAtSwap(Index);
			
			ReadyToBeReplaced.Add(Actor);
			ReadyToBeReplacedTransforms.Add(Transform);
		}
	}

	AActor* Pop()
	{
		return ObjectPool.Pop();
	}

	int ReadyToBeReplacedNum() const
	{
		return ReadyToBeReplaced.Num();
	}
	
	int GrowingObjectsNum() const
	{
		return GrowingObjects.Num();
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


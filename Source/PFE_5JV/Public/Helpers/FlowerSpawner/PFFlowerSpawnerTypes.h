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

USTRUCT()
struct FPFGrowingObjectData
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* ActorPtr;
	
	UPROPERTY()
	FTransform TargetTransform;
	
	UPROPERTY()
	float GrowthAlpha;
};

USTRUCT(Blueprintable, BlueprintType)
struct FPFPoolArrays
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AActor*> ObjectPool;

	UPROPERTY()
	TArray<FPFGrowingObjectData> GrowingObjectDatas;
	
	UPROPERTY()
	TArray<AActor*> ReadyToBeReplaced;
		
	UPROPERTY()
	TArray<FTransform> ReadyToBeReplacedTransforms;
	
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
		FPFGrowingObjectData GrowingObjectData;
		GrowingObjectData.ActorPtr = ActorToAdd;
		GrowingObjectData.TargetTransform = Transform;
		GrowingObjectData.GrowthAlpha = 0.f;
		GrowingObjectDatas.Add(GrowingObjectData);
	}

	void ReturnToPool()
	{
		ObjectPool.Append(ReadyToBeReplaced);
		ReadyToBeReplaced.Empty();
		ReadyToBeReplacedTransforms.Empty();
	}
	
	void TransferFromGrowingToReadyToBeReplaced(int Index)
	{
		if (GrowingObjectDatas.IsValidIndex(Index))
		{
			FPFGrowingObjectData GrowingObjectData = GrowingObjectDatas[Index];
			GrowingObjectDatas.RemoveAtSwap(Index);
			
			ReadyToBeReplaced.Add(GrowingObjectData.ActorPtr);
			ReadyToBeReplacedTransforms.Add(GrowingObjectData.TargetTransform);
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
		return GrowingObjectDatas.Num();
	}
	
};

USTRUCT(Blueprintable, BlueprintType)
struct FPFEnvironmentFlowers
{
	GENERATED_BODY()
	
	UPROPERTY()
	EPFFlowerEnvironment EnvironmentType = EPFFlowerEnvironment::EPFFS_None;
	
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
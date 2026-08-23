// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PFFlower.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Data/PFPaintResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFFlowerSpawnerResource.generated.h"

class UPFPhysicResource;
class UPFFlowerSpawnerResourceData;
class UHierarchicalInstancedStaticMeshComponent;
class UPFProximityResource;
class APFPainter;

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlowerColorChange, EPFFlowerColor, FlowerColor);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnActorsByHismSwitch, const TSubclassOf<AActor>&, const FLinearColor, const TArray<FTransform>&);
DECLARE_MULTICAST_DELEGATE(FOnFlowerSpawn);

UCLASS()
class PFE_5JV_API UPFFlowerSpawnerResource : public UPFResource
{
	GENERATED_BODY()

public :
	UPROPERTY(BlueprintAssignable, Category="FlowerSpawner")
	FOnFlowerColorChange OnFlowerColorChangeDelegate;
	
	FOnFlowerSpawn OnFlowerSpawnDelegate;
	FOnActorsByHismSwitch OnActorsByHismSwitchDelegate;

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	void SetCurrentFlowerColor(EPFFlowerColor FlowerColor);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	inline EPFFlowerColor GetCurrentFlowerColor() const {return CurrentFlowerColor_;}
		
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	bool TryGetFlowerColorFromEnum(EPFFlowerColor FlowerColor, FLinearColor& ColorValue);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner | Helper")
	static float GetCustomDataAlphaFromEnum(EPFCustomDataVersion Version);
	
protected:
	static constexpr int MaxActorsAmountPlaced = 20;
	
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	float DelayToSpawnTimer_ = -1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<APFPlayerCharacter> OwnerPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UPFProximityResource> ProximityResourcePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TObjectPtr<UPFFlowerSpawnerResourceData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TSubclassOf<class APFFlower> FlowerClass;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Painter|References")
	TObjectPtr<APFPainter> PainterPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Painter|References")
	TObjectPtr<UPFPaintResourceData> PainterDataPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Painter|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FlowerHISMPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	EPFFlowerColor CurrentFlowerColor_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	FLinearColor CurrentColorValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TObjectPtr<UMaterialParameterCollection> FlowerColorCollectionPtr_;

	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorldPtr_;	

	UPROPERTY()
	TObjectPtr<UPoolSubsystem> PoolSubsystemPtr_;
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	FVector GetRandomFlowerSize();

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	float GetRandomFlowerHeight(float GroundHeight);
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	bool CheckSpawnConditions(UPARAM(ref) const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
    float DetermineSpawnDelay();
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	void SpawnFlower();

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	FVector FindRandomPointInBrushRadius(float BrushRadius);

private:
	bool CheckValidity() const;
};

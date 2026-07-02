// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFlowerColorChange, EPFFlowerColor, FlowerColor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlowerSpawn);

/**
 * 
 */
UCLASS()
class PFE_5JV_API UPFFlowerSpawnerResource : public UPFResource
{
	GENERATED_BODY()

public :
	UPROPERTY(BlueprintAssignable, Category="FlowerSpawner")
	FOnFlowerColorChange OnFlowerColorChangeDelegate;

	UPROPERTY(BlueprintAssignable, Category="FlowerSpawner")
	FOnFlowerSpawn OnFlowerSpawnDelegate;

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	void SetCurrentFlowerColor(EPFFlowerColor FlowerColor);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	inline EPFFlowerColor GetCurrentFlowerColor() const {return CurrentFlowerColor_;}
		
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	bool TryGetFlowerColorFromEnum(EPFFlowerColor FlowerColor, FLinearColor& ColorValue);


protected:
	
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<APFPainter> PainterPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BlueFlowerHISMPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> RedFlowerHISMPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> YellowFlowerHISMPtr_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	EPFFlowerColor CurrentFlowerColor_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TObjectPtr<UMaterialParameterCollection> FlowerColorCollectionPtr_;
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	FVector GetRandomFlowerSize();

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	float GetRandomFlowerHeight(float GroundHeight);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	bool CheckSpawnConditions(UPARAM(ref) FHitResult& SupposedSpawnLocationHit, UPARAM(ref) FHitResult& InitialHit);

    UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
    float DetermineSpawnDelay();
	
private:
	bool CheckValidity() const;
};

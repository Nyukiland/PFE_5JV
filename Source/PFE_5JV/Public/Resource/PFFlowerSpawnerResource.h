// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/PFFlower.h"
#include "Data/PFPaintResourceData.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFFlowerSpawnerResource.generated.h"

struct FPFEnvironmentFlowers;
enum class EPFFlowerEnvironment : uint8;
enum class EPFFlowerColor : uint8;
class UPFPhysicResource;
class UPFFlowerSpawnerResourceData;
class UHierarchicalInstancedStaticMeshComponent;
class UPFProximityResource;
class APFPainter;
class APFFlower;


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
	
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	
protected:
	static constexpr int MaxActorsAmountPlaced = 20;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<APFPlayerCharacter> OwnerPtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	TObjectPtr<UPFProximityResource> ProximityResourcePtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TObjectPtr<UPFFlowerSpawnerResourceData> DataPtr_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner|References")
	TObjectPtr<UPFPaintResourceData> PainterDataPtr_;
		
	UPROPERTY()
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
	UPROPERTY()
	TObjectPtr<UWorld> OwnerWorldPtr_;	

	UPROPERTY()
	TObjectPtr<UPoolSubsystem> PoolSubsystemPtr_;
	
	UPROPERTY()
	TObjectPtr<APFPainter> PainterPtr_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner|References")
	float DelayToSpawnTimer_ = -1.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner")
	EPFFlowerColor CurrentFlowerColor_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="FlowerSpawner")
	FLinearColor CurrentColorValue_;
	
	// TODO : Vérifier que toujours nécessaire
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner")
	TSubclassOf<APFFlower> CurrentFlowerClassToSpawn_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="FlowerSpawner")
	EPFFlowerEnvironment CurrentEnvironment_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FlowerSpawner")
	TArray<FPFEnvironmentFlowers> FlowersByEnvironments;
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	FVector GetRandomFlowerSize();

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	float GetRandomFlowerHeight(float GroundHeight);
	
	TSubclassOf<APFFlower> GetRandomClassToSpawnAccordingToEnvironment(EPFFlowerEnvironment FlowerEnvironment);
	
	float GetSpeedGrowthAccordingToEnvironment(EPFFlowerEnvironment FlowerEnvironment) const;
		
	EPFFlowerEnvironment GetEnvironmentAccordingToClass(TSubclassOf<APFFlower> FlowerClass) const;
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	bool CheckSpawnConditions(UPARAM(ref) const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	void SetCurrentClassToSpawn(const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
    float DetermineSpawnDelay();
	
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	void SpawnFlower();

	// A plutôt mettre dans MathHelper
	UFUNCTION(BlueprintCallable, Category = "FlowerSpawner")
	FVector FindRandomPointInBrushRadius(float BrushRadius);
	
	FCollisionQueryParams CachedQueryParams;
	FCollisionObjectQueryParams CachedObjectQueryParams;
	
private:
	bool CheckValidity() const;
};

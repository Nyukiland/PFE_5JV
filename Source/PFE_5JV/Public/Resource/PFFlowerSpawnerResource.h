// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFFlowerSpawnerResource.generated.h"

class UPFFlowerSpawnerResourceData;
class UHierarchicalInstancedStaticMeshComponent;
class UPFProximityResource;
class APFPainter;

/**
 * 
 */
UCLASS()
class PFE_5JV_API UPFFlowerSpawnerResource : public UPFResource
{
	GENERATED_BODY()

public :
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;

protected:
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

private:
	bool CheckValidity() const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFFlowerSpawnerResource.generated.h"

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
	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<APFPlayerCharacter> OwnerPtr_;

	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<UPFProximityResource> ProximityResourcePtr_;

	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<APFPainter> PainterPtr_;

	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> BlueFlowerHISMPtr_;

	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> RedFlowerHISMPtr_;

	UPROPERTY(VisibleAnywhere, Category="FlowerSpawner|References")
	TObjectPtr<UHierarchicalInstancedStaticMeshComponent> YellowFlowerHISMPtr_;
};

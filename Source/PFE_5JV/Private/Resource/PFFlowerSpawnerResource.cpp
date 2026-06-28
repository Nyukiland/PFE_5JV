// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/PFPainter.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Resource/PFProximityResource.h"

void UPFFlowerSpawnerResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	OwnerPtr_ = ownerObj;
	ProximityResourcePtr_ = OwnerPtr_->GetStateComponent<UPFProximityResource>();
	
	// PainterPtr_ = ;
	//
	// TArray<UActorComponent*> BlueFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("BlueFlower"));
	// if (BlueFlowerHISMComponents.Num() > 0) BlueFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(BlueFlowerHISMComponents[0]);
	//
	// TArray<UActorComponent*> RedFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("RedFlower"));
	// if (RedFlowerHISMComponents.Num() > 0) RedFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(RedFlowerHISMComponents[0]);
	//
	// TArray<UActorComponent*> YellowFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("YellowFlower"));
	// if (YellowFlowerHISMComponents.Num() > 0) YellowFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(YellowFlowerHISMComponents[0]);
}	

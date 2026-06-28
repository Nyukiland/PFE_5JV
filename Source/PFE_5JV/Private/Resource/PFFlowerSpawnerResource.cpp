// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/PFPainter.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Resource/PFProximityResource.h"
#include "Resource/Data/PFFlowerSpawnerResourceData.h"

void UPFFlowerSpawnerResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	OwnerPtr_ = ownerObj;
	ProximityResourcePtr_ = OwnerPtr_->GetStateComponent<UPFProximityResource>();
	
	PainterPtr_ = APFPainter::GetPainter(OwnerPtr_->GetWorld());
	
	TArray<UActorComponent*> BlueFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("BlueFlower"));
	if (BlueFlowerHISMComponents.Num() > 0) BlueFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(BlueFlowerHISMComponents[0]);
	
	TArray<UActorComponent*> RedFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("RedFlower"));
	if (RedFlowerHISMComponents.Num() > 0) RedFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(RedFlowerHISMComponents[0]);
	
	TArray<UActorComponent*> YellowFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("YellowFlower"));
	if (YellowFlowerHISMComponents.Num() > 0) YellowFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(YellowFlowerHISMComponents[0]);

	if (!CheckValidity()) return;
}

bool UPFFlowerSpawnerResource::CheckValidity() const
{
	if (!OwnerPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The ObjectPtr is NULL"));
		return false;
	}

	if (!ProximityResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The ProximityResourcePtr_ referenced in FlowerSpawnerResource blueprint is NULL"))
		return false;
	}
	
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The Data referenced in FlowerSpawnerResource blueprint is NULL"));
		return false;
	}

	if (!DataPtr_->SpawnDelayBasedOnVelocityCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The Data Curve in FlowerSpawnerResourceData blueprint is NULL"))
		return false;
	}
	
	if (!PainterPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The PainterPtr is NULL"))
		return false;
	}
	
	if (!BlueFlowerHISMPtr_ || !RedFlowerHISMPtr_ || !YellowFlowerHISMPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] One or more HISM Flower references are NULL"));
		return false;
	}
	
	return true;
}	

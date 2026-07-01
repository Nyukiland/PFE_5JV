// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/PFPainter.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialParameterCollection.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Resource/PFProximityResource.h"
#include "Resource/PFPhysicResource.h"
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

	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();

	CurrentFlowerColor_ = EPFFlowerColor::EPFFC_None;

	if (!CheckValidity()) return;
}

void UPFFlowerSpawnerResource::SetCurrentFlowerColor(EPFFlowerColor FlowerColor)
{
	CurrentFlowerColor_ = FlowerColor;
	OnFlowerColorChangeDelegate.Broadcast(FlowerColor);
}

bool UPFFlowerSpawnerResource::TryGetFlowerColorFromEnum(EPFFlowerColor FlowerColor, FLinearColor& ColorValue)
{
	bool bParameterFound = false;
	switch (FlowerColor)
	{
	case EPFFlowerColor::EPFFC_Blue:
		ColorValue = FlowerColorCollectionPtr_->GetVectorParameterDefaultValue("S_Blue", bParameterFound);
		return bParameterFound;
		
		case EPFFlowerColor::EPFFC_Red:
			ColorValue = FlowerColorCollectionPtr_->GetVectorParameterDefaultValue("S_Red", bParameterFound);
			return bParameterFound;

		case EPFFlowerColor::EPFFC_Yellow:
			ColorValue = FlowerColorCollectionPtr_->GetVectorParameterDefaultValue("S_Yellow", bParameterFound);
			return bParameterFound;

		case EPFFlowerColor::EPFFC_Purple:
			ColorValue = FlowerColorCollectionPtr_->GetVectorParameterDefaultValue("S_Purple", bParameterFound);
			return bParameterFound;

		case EPFFlowerColor::EPFFC_None:
		default:
			return bParameterFound;

	}
}


FVector UPFFlowerSpawnerResource::GetRandomFlowerSize()
{
	const float RandomFlowerSize = FMath::RandRange(DataPtr_->MinimumFlowerScale, DataPtr_->MaximumFlowerScale);	
	const FVector RandomFlowerScale = FVector(RandomFlowerSize);
	
	return RandomFlowerScale;
}

bool UPFFlowerSpawnerResource::CheckSpawnConditions(FHitResult& SupposedSpawnLocationHit, FHitResult& InitialHit)
{
	if (!SupposedSpawnLocationHit.IsValidBlockingHit()) return false;
	if (!SupposedSpawnLocationHit.GetActor()) return false;

	// Si la localisation du spawn n'est pas sur le même acteur que le hit initial, on ne spawn pas : 
	if(SupposedSpawnLocationHit.GetActor() != InitialHit.GetActor()) return false;

	// Si la pente de la surface où on veut spawn est trop raide (ex : falaise), on ne spawn pas :
	// Calcule l'angle de la pente :
	FVector UpVector = FVector(0,0,1);
	const double CosTheta = FVector::DotProduct(SupposedSpawnLocationHit.ImpactNormal, UpVector);
	double SlopAngle = FMath::Acos(CosTheta);
	SlopAngle = FMath::RadiansToDegrees(SlopAngle);
	if(SlopAngle >= DataPtr_->MaximalSlopInDegreesToSpawn) return false;
	
	// Ne spawn pas sur les supports n'ayant pas le tag "Landscape"
	if(SupposedSpawnLocationHit.GetActor()->ActorHasTag("Landscape") == false) return false;
	
	// Dans les autres cas, on spawn la fleur :
	return true;
}

float UPFFlowerSpawnerResource::DetermineSpawnDelay()
{
	float PlayerVelocityPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
	float SpawnDelay = FMath::Lerp(PlayerVelocityPercentage, DataPtr_->DelayBetweenTwoSpawnsAtMinimalVelocity, DataPtr_->DelayBetweenTwoSpawnsAtMaximalVelocity);
	return SpawnDelay;
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
	
	if (!PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The PhysicResourcePtr is NULL"));
		return false;
	}

	if (!FlowerColorCollectionPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The FlowerColorCollectionPtr is NULL"));
		return false;
	}
	
	return true;
}	

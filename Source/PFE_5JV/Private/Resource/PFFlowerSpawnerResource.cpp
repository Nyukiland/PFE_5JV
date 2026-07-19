// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/PFMathHelper.h"
#include "Helpers/PFPainter.h"
#include "Kismet/KismetMathLibrary.h"
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

	OwnerWorldPtr_ = Owner->GetWorld();

	CurrentFlowerColor_ = EPFFlowerColor::EPFFC_None;
	OnFlowerSpawnDelegate.AddDynamic(this, &UPFFlowerSpawnerResource::SpawnFlowerC);
	if (!CheckValidity()) return;
}

void UPFFlowerSpawnerResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if(DelayToSpawnTimer_ < 0.f)
	{
		DelayToSpawnTimer_ = DetermineSpawnDelay();
		OnFlowerSpawnDelegate.Broadcast();
	}
	if(DelayToSpawnTimer_ >= 0.f) DelayToSpawnTimer_ -= deltaTime;
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

float UPFFlowerSpawnerResource::GetRandomFlowerHeight(float GroundHeight)
{
	float RandomFlowerHeight = FMath::RandRange(DataPtr_->MinimalHeightAboveGround, DataPtr_->MaximalHeightAboveGround);	
	RandomFlowerHeight += GroundHeight;
	
	return RandomFlowerHeight;
}

bool UPFFlowerSpawnerResource::CheckSpawnConditions(const FHitResult& Hit)
{
	if (!Hit.bBlockingHit) return false;
	if (!Hit.GetActor()) return false;

	// Si la pente de la surface où on veut spawn est trop raide (ex : falaise), on ne spawn pas :
	// Calcule l'angle de la pente :
	FVector UpVector = FVector(0,0,1);
	const double CosTheta = FVector::DotProduct(Hit.ImpactNormal, UpVector);
	double SlopAngle = FMath::Acos(CosTheta);
	SlopAngle = FMath::RadiansToDegrees(SlopAngle);
	if(SlopAngle >= DataPtr_->MaximalSlopInDegreesToSpawn) return false;
	
	// Ne spawn pas sur les supports n'ayant pas le tag "Landscape"
	if(Hit.GetActor()->ActorHasTag("Landscape") == false) return false;
	
	// Dans les autres cas, on spawn la fleur :
	return true;
}

float UPFFlowerSpawnerResource::DetermineSpawnDelay()
{
	float PlayerVelocityPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
	// float PlayerVelocityPercentage = 0.f;

	float SpawnDelay = FMath::Lerp(DataPtr_->DelayBetweenTwoSpawnsAtMinimalVelocity, DataPtr_->DelayBetweenTwoSpawnsAtMaximalVelocity, PlayerVelocityPercentage);

	return SpawnDelay;
}

void UPFFlowerSpawnerResource::SpawnFlowerC()
{
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Flower Spawn !!!!!")));

	// // Si la couleur de la fleur n'est pas set, pas la peine de continuer plus loin
	// if(CurrentFlowerColor_ == EPFFlowerColor::EPFFC_None) return;
	//
	// TArray<FHitResult> ValidHitResults = ProximityResourcePtr_->ValidHitResults;
	// if(ValidHitResults.IsEmpty()) return;
	//
	// FHitResult InitialHitResult;
	//
	// for(const FHitResult& ValidHitResult: ValidHitResults)
	// {
	// 	if(!CheckSpawnConditions(ValidHitResult)) continue;
	// 	InitialHitResult = ValidHitResult;
	// 	break;
	// }
	//
	// // Rotate the plan XY with random result to be perpendicular to the impact normal :
	// FVector PlayerPosition = OwnerPtr_->GetActorLocation(); 
	// FVector BirdToInitialHitVector = PlayerPosition - InitialHitResult.ImpactPoint;
	// float Distance = BirdToInitialHitVector.Size();
	//
	// float BrushRadius = UPFMathHelper::RemapClamped(Distance, PainterDataPtr_->BrushMaxDistance, 0.0f, PainterDataPtr_->BrushSize.X, PainterDataPtr_->BrushSize.Y);
	// BrushRadius *= 18.0f;
	//
	// // TODO : FONCTION POUR AVOIR UN VRAI RANDOM POINT
	// FVector RandomPointInBrushRadius = FVector(1.0f,0.0f,0.0f);
	//
	// FVector UpVector =  FVector(0.0f, 0.0f, 1.0f);
	// FRotator UpToNormalRotation = UKismetMathLibrary::MakeRotFromX(InitialHitResult.ImpactNormal - UpVector);
	// FVector InBrushPointPerpendicularToNormal = UpToNormalRotation.RotateVector(RandomPointInBrushRadius); 
	//
	// // Place the plan with random result at the impact location :
	// FVector BrushPlanAtImpactLocation = InitialHitResult.ImpactPoint + InBrushPointPerpendicularToNormal;
	//
	// // Get the normalized direction between the bird and the random direction :
	// FVector BirdToRandomLocationInBrushVector = BrushPlanAtImpactLocation - PlayerPosition;
	// FVector NormalizedBirdToRandomLocationInBrushVector = BirdToRandomLocationInBrushVector.GetSafeNormal();
	//
	// // Adjust the length :
	// FVector LengthenVector = NormalizedBirdToRandomLocationInBrushVector * 5000.f; // Valeur à mettre dans les datas pour les GDs
	//
	// // Get normal and impact datas for this point :
	// FHitResult CurrentHitResult;
	// const FCollisionShape sphere = FCollisionShape::MakeSphere(1.f);
	// OwnerWorldPtr_->SweepSingleByChannel(CurrentHitResult, PlayerPosition, PlayerPosition + LengthenVector,
	// 								FQuat::Identity, ECC_Visibility, sphere);
	//
	// if(!CheckSpawnConditions(CurrentHitResult)) return;
	// float FlowerHeight = GetRandomFlowerHeight(CurrentHitResult.ImpactPoint.Z);
	// FVector FlowerSize = GetRandomFlowerSize();
	//
	// FVector SpawnLocation = FVector(CurrentHitResult.ImpactPoint.X, CurrentHitResult.ImpactPoint.Y, FlowerHeight);
	// FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromZX(CurrentHitResult.ImpactNormal, UpVector);
	// GetWorld()->SpawnActor<class AActor>(FlowerClass, SpawnLocation, SpawnRotation); // Faire une classe C++ pour la fleur
	//

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

	if (!PainterDataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The PainterDataPtr is NULL"))
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

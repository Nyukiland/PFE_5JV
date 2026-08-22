// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "Actors/PoolSubsystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/PFMathHelper.h"
#include "Helpers/PFPainter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Resource/PFProximityResource.h"
#include "Resource/PFPhysicResource.h"
#include "Math/UnrealMathUtility.h"
#include "Resource/Data/PFFlowerSpawnerResourceData.h"

void UPFFlowerSpawnerResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	OwnerPtr_ = ownerObj;
	OwnerWorldPtr_ = Owner->GetWorld();
	
	ProximityResourcePtr_ = OwnerPtr_->GetStateComponent<UPFProximityResource>();
	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();
	PainterPtr_ = APFPainter::GetPainter(OwnerPtr_->GetWorld());
		
	// Initialize Actors to Spawn
	PoolSubsystemPtr_ = GetWorld()->GetSubsystem<UPoolSubsystem>();
	// Flower :
	PoolSubsystemPtr_->InitializePool(FlowerClass, PoolSubsystemPtr_->InitialPoolSize);


	TArray<UActorComponent*> FlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("Flower_0"));
	if (FlowerHISMComponents.Num() > 0) FlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(FlowerHISMComponents[0]);

	CurrentFlowerColor_ = EPFFlowerColor::EPFFC_None;
	OnFlowerSpawnDelegate.AddDynamic(this, &UPFFlowerSpawnerResource::SpawnFlower);
	if (!CheckValidity()) return;
}

void UPFFlowerSpawnerResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if(DelayToSpawnTimer_ < 0.f)
	{
		DelayToSpawnTimer_ = DetermineSpawnDelay();
		// Si la couleur de la fleur n'est pas set, pas la peine de continuer plus loin
		if(CurrentFlowerColor_ != EPFFlowerColor::EPFFC_None) OnFlowerSpawnDelegate.Broadcast();
	}
	if(DelayToSpawnTimer_ >= 0.f) DelayToSpawnTimer_ -= deltaTime;

	// Pour chaque type d'acteurs qui a un pool, si on a atteint la limite d'acteurs placés, on remplace par la version HISM :
	for (const auto& Pair : PoolSubsystemPtr_->ObjectPools)
	{
		UClass* PoolClass = Pair.Key;
		FPoolArrays ObjectPool = Pair.Value;
		
		if(ObjectPool.PlacedObjectsNum() >= MaxActorsAmountPlaced) {
			if(FlowerHISMPtr_ == nullptr) return;
			FLinearColor ColorValue;
			TryGetFlowerColorFromEnum(CurrentFlowerColor_, ColorValue);
			
			TArray<int32> Indices = FlowerHISMPtr_->AddInstances(ObjectPool.PlacedObjectTransforms, true, true, false
				);

			for(int32 InstanceIndex : Indices)
			{
				FlowerHISMPtr_->SetCustomDataValue(InstanceIndex, 0, ColorValue.R, true);
				FlowerHISMPtr_->SetCustomDataValue(InstanceIndex, 1, ColorValue.G, true);
				FlowerHISMPtr_->SetCustomDataValue(InstanceIndex, 2, ColorValue.B, true);
			}
			PoolSubsystemPtr_->ReturnToPool(PoolClass);
		}
	}
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

void UPFFlowerSpawnerResource::SpawnFlower()
{
	TArray<FHitResult> ValidHitResults = ProximityResourcePtr_->ValidHitResults;
	if(ValidHitResults.IsEmpty()) return;

	FHitResult InitialHitResult;
	
	for(const FHitResult& ValidHitResult: ValidHitResults)
	{
		if(!CheckSpawnConditions(ValidHitResult)) continue;
		InitialHitResult = ValidHitResult;
		break;
	}

	// Rotate the plan XY with random result to be perpendicular to the impact normal :
	FVector PlayerPosition = OwnerPtr_->GetActorLocation(); 
	FVector BirdToInitialHitVector = PlayerPosition - InitialHitResult.ImpactPoint;
	float Distance = BirdToInitialHitVector.Size();
	
	float BrushRadius = UPFMathHelper::RemapClamped(Distance, PainterDataPtr_->BrushMaxDistance, 0.0f, PainterDataPtr_->BrushSize.X, PainterDataPtr_->BrushSize.Y);
	BrushRadius *= 18.0f;

	FVector RandomPointInBrushRadius = FindRandomPointInBrushRadius(BrushRadius);
	
	FVector UpVector =  FVector(0.0f, 0.0f, 1.0f);
	FRotator UpToNormalRotation = UKismetMathLibrary::MakeRotFromX(InitialHitResult.ImpactNormal - UpVector);
	FVector InBrushPointPerpendicularToNormal = UpToNormalRotation.RotateVector(RandomPointInBrushRadius); 

	// Place the plan with random result at the impact location :
	FVector BrushPlanAtImpactLocation = InitialHitResult.ImpactPoint + InBrushPointPerpendicularToNormal;

	// Get the normalized direction between the bird and the random direction :
	FVector BirdToRandomLocationInBrushVector = BrushPlanAtImpactLocation - PlayerPosition;
	FVector NormalizedBirdToRandomLocationInBrushVector = BirdToRandomLocationInBrushVector.GetSafeNormal();

	// Adjust the length :
	FVector LengthenVector = NormalizedBirdToRandomLocationInBrushVector * DataPtr_->MaximalSpawnDistanceFromBird; 

	// Get normal and impact datas for this point :
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	queryParams.bTraceComplex = true; 
	FHitResult CurrentHitResult;
	const FCollisionShape sphere = FCollisionShape::MakeSphere(1.f);
	OwnerWorldPtr_->SweepSingleByChannel(CurrentHitResult, PlayerPosition, PlayerPosition + LengthenVector,
									FQuat::Identity, ECC_Visibility, sphere, queryParams);
	
	if(!CheckSpawnConditions(CurrentHitResult)) return;
	
	float FlowerHeight = GetRandomFlowerHeight(CurrentHitResult.ImpactPoint.Z);
	FVector FlowerSize = GetRandomFlowerSize();
	
	FVector SpawnLocation = FVector(CurrentHitResult.ImpactPoint.X, CurrentHitResult.ImpactPoint.Y, FlowerHeight);
	FRotator SpawnRotation = UKismetMathLibrary::MakeRotFromZX(CurrentHitResult.ImpactNormal, UpVector);


	// Spawn avec PoolSystem : 
	APFFlower* Flower = Cast<APFFlower>(PoolSubsystemPtr_->SpawnFromPool<AActor>(FlowerClass, SpawnLocation, SpawnRotation));

	// Spawn sans PoolSystem
	// APFFlower* Flower = GetWorld()->SpawnActor<class APFFlower>(FlowerClass, SpawnLocation, SpawnRotation); 
	
	Flower->SetActorScale3D(FlowerSize);
	
	FLinearColor ColorValue;
	TryGetFlowerColorFromEnum(CurrentFlowerColor_, ColorValue);
	
	// GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta : %f - %f - %f"), ColorValue.R, ColorValue.G, ColorValue.B));
	// if(Flower->GetFlowerMesh() == nullptr)	UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] GetFlowerMesh is NULL"));
	// Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(0, ColorValue.R);
	// Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(1, ColorValue.G);
	// Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(2, ColorValue.B);
	// Flower->GetFlowerMesh()->SetCustomPrimitiveDataVector4f(0, ColorValue);
	
	UMaterialInstanceDynamic* FlowerMaterial = Flower->GetDynamicMaterial();
	if (!FlowerMaterial) return;
	FlowerMaterial->SetVectorParameterValue(FName("FlowerColor"), ColorValue);
	// if(FlowerMaterial == nullptr)	UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] FlowerMaterial is NULL"));
	// FlowerMaterial->SetScalarParameterByIndex(0, ColorValue.R);
	// FlowerMaterial->SetScalarParameterByIndex(1, ColorValue.G);
	// FlowerMaterial->SetScalarParameterByIndex(2, ColorValue.B);

}

FVector UPFFlowerSpawnerResource::FindRandomPointInBrushRadius(float BrushRadius)
{
	// Determine randomness 
	float Seed = UGameplayStatics::GetRealTimeSeconds(GetWorld()) + OwnerPtr_->GetTransform().GetLocation().X + OwnerPtr_->GetTransform().GetLocation().Y + OwnerPtr_->GetTransform().GetLocation().Z; 
	const FRandomStream RandomStream(Seed);

	// Determine R :
	float RRandomNumber = RandomStream.RandRange(0.f, 1000.f);
	RRandomNumber *= 0.001f;
	float R = BrushRadius * FMath::Sqrt(RRandomNumber);
	
	// Determine Theta angle
	float ThetaRandomNumber = RandomStream.RandRange(0.f, 1000.f);
	ThetaRandomNumber *= 0.001f;
	float Theta  = ThetaRandomNumber * 2 * PI; // for PI, need to add :  #include "Math/UnrealMathUtility.h"

	// Determine XCoordinate
	float XCoordinate = R * cos(Theta);

	// Determine YCoordinate
	float YCoordinate = R * sin(Theta);

	return FVector(XCoordinate, YCoordinate, 0.f);
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

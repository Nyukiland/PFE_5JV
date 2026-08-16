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
	ProximityResourcePtr_ = OwnerPtr_->GetStateComponent<UPFProximityResource>();
	
	PainterPtr_ = APFPainter::GetPainter(OwnerPtr_->GetWorld());
		
	TArray<UActorComponent*> BlueFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("BlueFlower"));
	if (BlueFlowerHISMComponents.Num() > 0) BlueFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(BlueFlowerHISMComponents[0]);
	FLinearColor BlueValue;
	TryGetFlowerColorFromEnum(EPFFlowerColor::EPFFC_Blue, BlueValue);
	FVector BlueFlowerColor = FVector(BlueValue);
	BlueFlowerHISMPtr_->SetVectorParameterValueOnMaterials(FName("FlowerColor"), BlueFlowerColor);
			
	TArray<UActorComponent*> RedFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("RedFlower"));
	if (RedFlowerHISMComponents.Num() > 0) RedFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(RedFlowerHISMComponents[0]);
	FLinearColor RedValue;
	TryGetFlowerColorFromEnum(EPFFlowerColor::EPFFC_Red, RedValue);
	FVector RedFlowerColor = FVector(RedValue);
	RedFlowerHISMPtr_->SetVectorParameterValueOnMaterials(FName("FlowerColor"), RedFlowerColor);
	
	TArray<UActorComponent*> YellowFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("YellowFlower"));
	if (YellowFlowerHISMComponents.Num() > 0) YellowFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(YellowFlowerHISMComponents[0]);
	FLinearColor YellowValue;
	TryGetFlowerColorFromEnum(EPFFlowerColor::EPFFC_Yellow, YellowValue);
	FVector YellowFlowerColor = FVector(YellowValue);
	YellowFlowerHISMPtr_->SetVectorParameterValueOnMaterials(FName("FlowerColor"), YellowFlowerColor);

	TArray<UActorComponent*> PurpleFlowerHISMComponents = PainterPtr_->GetComponentsByTag(UHierarchicalInstancedStaticMeshComponent::StaticClass(), FName("PurpleFlower"));
	if (PurpleFlowerHISMComponents.Num() > 0) PurpleFlowerHISMPtr_ = Cast<UHierarchicalInstancedStaticMeshComponent>(PurpleFlowerHISMComponents[0]);
	FLinearColor PurpleValue;
	TryGetFlowerColorFromEnum(EPFFlowerColor::EPFFC_Purple, PurpleValue);
	FVector PurpleFlowerColor = FVector(YellowValue);
	YellowFlowerHISMPtr_->SetVectorParameterValueOnMaterials(FName("FlowerColor"), PurpleFlowerColor);
	
	
	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();

	OwnerWorldPtr_ = Owner->GetWorld();

	PoolSubsystemPtr_ = GetWorld()->GetSubsystem<UPoolSubsystem>();
	PoolSubsystemPtr_->ImplementInitialPool(FlowerClass, DataPtr_->InitialPoolSize);

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

	// Si on a atteint la limite d'objets de la pool placé, on remplace par HISM :
	if(PoolSubsystemPtr_->PlacedObjects.Num() >= DataPtr_->ActorsAmountSpawnedBeforeReplacingByHism) {
		UHierarchicalInstancedStaticMeshComponent* FlowerHISMPtr_ = GetColoredHISM();
		if(FlowerHISMPtr_ == nullptr) return;
		FLinearColor ColorValue;
		TryGetFlowerColorFromEnum(CurrentFlowerColor_, ColorValue);
		FVector FlowerColor = FVector(ColorValue);
		FlowerHISMPtr_->SetVectorParameterValueOnMaterials(FName("FlowerColor"), FlowerColor);
		FlowerHISMPtr_->AddInstances(PoolSubsystemPtr_->PlacedObjectTransforms, false, true, false
			);
		PoolSubsystemPtr_->PlacedObjectTransforms.Empty();
		PoolSubsystemPtr_->ReturnToPool(FlowerClass);
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

UHierarchicalInstancedStaticMeshComponent* UPFFlowerSpawnerResource::GetColoredHISM()
{
	switch (CurrentFlowerColor_)
	{
	case EPFFlowerColor::EPFFC_Blue:
		return BlueFlowerHISMPtr_;
		
	case EPFFlowerColor::EPFFC_Red:
		return RedFlowerHISMPtr_;

	case EPFFlowerColor::EPFFC_Yellow:
		return YellowFlowerHISMPtr_;

	case EPFFlowerColor::EPFFC_Purple:
		return PurpleFlowerHISMPtr_;

	case EPFFlowerColor::EPFFC_None:
	default:
		return nullptr;
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
	APFFlower* Flower = Cast<APFFlower>(PoolSubsystemPtr_->SpawnFromPool(FlowerClass, SpawnLocation, SpawnRotation));

	// Spawn sans PoolSystem
	// APFFlower* Flower = GetWorld()->SpawnActor<class APFFlower>(FlowerClass, SpawnLocation, SpawnRotation); 
	
	Flower->SetActorScale3D(FlowerSize);
	
	FLinearColor ColorValue;
	TryGetFlowerColorFromEnum(CurrentFlowerColor_, ColorValue);
	
	UMaterialInstanceDynamic* FlowerMaterial = Flower->GetDynamicMaterial();
	if (!FlowerMaterial) return;
	FlowerMaterial->SetVectorParameterValue(FName("FlowerColor"), ColorValue);
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

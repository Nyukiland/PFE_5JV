// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

#include "FrameTypes.h"
#include "Actors/PoolSubsystem.h"
#include "Helpers/PFMathHelper.h"
#include "Helpers/PFPainter.h"
#include "Helpers/FlowerSpawner/FlowerSpawnerHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Resource/PFProximityResource.h"
#include "Resource/PFPhysicResource.h"
#include "Math/UnrealMathUtility.h"
#include "Resource/Data/PFFlowerSpawnerResourceData.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"

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
	
	if (!CheckValidity()) return;
	
	// Flowers :
	TMap<UClass*, FPFStaticMeshModelData> StaticMeshModelsToSpawn = PainterPtr_->GetStaticMeshModelsToSpawn();
	for (const auto& Pair : StaticMeshModelsToSpawn)
	{
		UClass* FlowerClass = Pair.Key;
		FPFStaticMeshModelData StaticMeshModelData = Pair.Value;
		
		EPFFlowerEnvironment FlowerEnvironment = StaticMeshModelData.EnvironmentType;
		FPFEnvironmentFlowers& FlowersByEnvironment = FlowersByEnvironments.FindOrAdd(FlowerEnvironment);
		FlowersByEnvironment.AddUnique(FlowerClass);
		PoolSubsystemPtr_->InitializePool(FlowerClass, PoolSubsystemPtr_->InitialPoolSize);
	}
	
	for (const auto& Pair : FlowersByEnvironments)
	{
		EPFFlowerEnvironment FlowerEnvironment = Pair.Key;
		FPFEnvironmentFlowers FlowersList = Pair.Value;
		
		for (const auto& FlowerClass : FlowersList.FlowerClasses)
		{
			FName EnvironmentName = UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(FlowerEnvironment);
			FString EnvironmentString = EnvironmentName.ToString();
			FString FlowerClassString = FlowerClass->GetName();
			UE_LOG(LogTemp, Warning, TEXT("[FlowerSpawner] Environment : %s - FlowerClass : %s"), *EnvironmentString, *FlowerClassString);
		}
	}
	
	CachedQueryParams.AddIgnoredActor(Owner);
	CachedQueryParams.bTraceComplex = true;
	CachedQueryParams.bReturnFaceIndex = true;

	CachedObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	CachedObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel4);
		
	CurrentFlowerColor_ = EPFFlowerColor::EPFFC_None;
	OnFlowerSpawnDelegate.AddUObject(this, &UPFFlowerSpawnerResource::SpawnFlower);
	OnActorsByHismSwitchDelegate.AddUObject(PainterPtr_, &APFPainter::ReplaceActorsByHismByClass);
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

	for (auto& Pair : PoolSubsystemPtr_->ObjectPools)
	{
		UClass* PoolClass = Pair.Key;
		FPFPoolArrays& ObjectPool = Pair.Value;

		if(ObjectPool.GrowingObjectsNum() > 0)
		{
			for (int i = ObjectPool.GrowingObjectsNum() -1 ; i >= 0; i--)
			{
				FPFGrowingObjectData& GrowingObjectData = ObjectPool.GrowingObjectDatas[i];
				GrowingObjectData.GrowthAlpha += deltaTime * 1; // * multiplicateur de vitesse ? 
				GrowingObjectData.GrowthAlpha = FMath::Clamp(GrowingObjectData.GrowthAlpha, 0.f, 1.f);
				FVector NewScale = FMath::Lerp(FVector::ZeroVector, GrowingObjectData.TargetTransform.GetScale3D(), GrowingObjectData.GrowthAlpha);
				GrowingObjectData.ActorPtr->SetActorScale3D(NewScale);
				
				if (GrowingObjectData.GrowthAlpha >= 1.f)
				{
					ObjectPool.TransferFromGrowingToReadyToBeReplaced(i);
				}
			}
			
			// Pour chaque type d'acteurs qui a un pool, si on a atteint la limite d'acteurs placés, on remplace par la version HISM :
			if(ObjectPool.ReadyToBeReplacedNum() >= MaxActorsAmountPlaced) {
				OnActorsByHismSwitchDelegate.Broadcast(PoolClass, CurrentColorValue_, ObjectPool.ReadyToBeReplacedTransforms);
				PoolSubsystemPtr_->ReturnToPool(PoolClass);
			}
		}
	}
}

void UPFFlowerSpawnerResource::SetCurrentFlowerColor(EPFFlowerColor FlowerColor)
{
	// if it is already the same color, do nothing
	if(CurrentFlowerColor_ == FlowerColor) return;

	// if there is actors placed with old color, replace by HISM version before changing the color
	for (const auto& Pair : PoolSubsystemPtr_->ObjectPools)
	{
		UClass* PoolClass = Pair.Key;
		FPFPoolArrays ObjectPool = Pair.Value;

		if(ObjectPool.ReadyToBeReplacedIsEmpty() == false)
		{
			OnActorsByHismSwitchDelegate.Broadcast(PoolClass, CurrentColorValue_, ObjectPool.ReadyToBeReplacedTransforms);
		}
	}
	
	// Implement new color :
	CurrentFlowerColor_ = FlowerColor;
	UFlowerSpawnerHelper::TryGetFlowerColorFromEnum(CurrentFlowerColor_, CurrentColorValue_);
	OnFlowerColorChangeDelegate.Broadcast(CurrentFlowerColor_);
}

FVector UPFFlowerSpawnerResource::GetRandomFlowerSize()
{
	float MinimalScale;
	float MaximalScale;
	
	switch (CurrentEnvironment_)
	{
	case EPFFlowerEnvironment::EPFFS_Landscape:
		MinimalScale = DataPtr_->MinimumFlowerScale;
		MaximalScale = DataPtr_->MaximumFlowerScale;
		break;
	case EPFFlowerEnvironment::EPFFS_Water:
		MinimalScale = DataPtr_->MinimumWaterLilyScale;
		MaximalScale = DataPtr_->MaximumWaterLilyScale;
		break;
	case EPFFlowerEnvironment::EPFFS_Cliff:
		MinimalScale = DataPtr_->MinimumIvyScale;
		MaximalScale = DataPtr_->MaximumIvyScale;
		break;
	case EPFFlowerEnvironment::EPFFS_None:
	default:
		MinimalScale = 1.f;
		MaximalScale = 1.f;
		break;
	}
	
	const float RandomFlowerSize = FMath::RandRange(MinimalScale, MaximalScale);	
	const FVector RandomFlowerScale = FVector(RandomFlowerSize);
	
	return RandomFlowerScale;
}

float UPFFlowerSpawnerResource::GetRandomFlowerHeight(float GroundHeight)
{
	float MinimalHeight;
	float MaximalHeight;
	
	switch (CurrentEnvironment_)
	{
	case EPFFlowerEnvironment::EPFFS_Landscape:
		MinimalHeight = DataPtr_->MinimalHeightAboveGroundForFlower;
		MaximalHeight = DataPtr_->MaximalHeightAboveGroundForFlower;
		break;
	case EPFFlowerEnvironment::EPFFS_Water:
		MinimalHeight = DataPtr_->MinimalHeightAboveGroundForWaterLily;
		MaximalHeight = DataPtr_->MaximalHeightAboveGroundForWaterLily;
		break;
	case EPFFlowerEnvironment::EPFFS_Cliff:
		MinimalHeight = DataPtr_->MinimalHeightAboveGroundForIvy;
		MaximalHeight = DataPtr_->MaximalHeightAboveGroundForIvy;
		break;
	case EPFFlowerEnvironment::EPFFS_None:
	default:
		MinimalHeight = 1.f;
		MaximalHeight = 1.f;
		break;
	}
	
	float RandomFlowerHeight = FMath::RandRange(MinimalHeight, MaximalHeight);	
	RandomFlowerHeight += GroundHeight;
	
	return RandomFlowerHeight;
}

TSubclassOf<APFFlower> UPFFlowerSpawnerResource::GetRandomClassToSpawnAccordingToEnvironment(EPFFlowerEnvironment Environment)
{
	FPFEnvironmentFlowers* EnvironmentFlowers = FlowersByEnvironments.Find(Environment);
	TArray<TSubclassOf<APFFlower>> FlowerClasses = EnvironmentFlowers->FlowerClasses;
	
	int MaxRange = FlowerClasses.Num() - 1;
	int RandomFlowerClassIndex = FMath::RandRange(0, MaxRange);
	TSubclassOf<APFFlower> Class = FlowerClasses[RandomFlowerClassIndex];
	FString ClassString = Class->GetName();
	
	return FlowerClasses[RandomFlowerClassIndex];
}

bool UPFFlowerSpawnerResource::CheckSpawnConditions(const FHitResult& Hit)
{
	if (!Hit.bBlockingHit) return false;
	if (!Hit.GetActor()) return false;

	return true;
}

void UPFFlowerSpawnerResource::SetCurrentClassToSpawn(const FHitResult& Hit)
{
	if (
		Hit.GetActor() and
		Hit.GetActor()->ActorHasTag(UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(EPFFlowerEnvironment::EPFFS_Landscape)) == true)
	{
		// Calcule l'angle de la pente :
		FVector UpVector = FVector(0,0,1);
		const double CosTheta = FVector::DotProduct(Hit.ImpactNormal, UpVector);
		double SlopAngle = FMath::Acos(CosTheta);
		SlopAngle = FMath::RadiansToDegrees(SlopAngle);
		
		if(SlopAngle >= DataPtr_->MaximalSlopInDegreesToSpawn)
		{
			CurrentEnvironment_ = EPFFlowerEnvironment::EPFFS_Cliff;
		} 
		else
		{
			CurrentEnvironment_ = EPFFlowerEnvironment::EPFFS_Landscape;
		}
	} 
	// else if (Hit.GetActor()->ActorHasTag(UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(EPFFlowerEnvironment::EPFFS_Water)) == true)
	else if (
		(Hit.GetComponent() and
		Hit.GetComponent()->GetCollisionProfileName() == "WaterBodyCollision") or 
		(Hit.GetActor() and
		Hit.GetActor()->ActorHasTag(UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(EPFFlowerEnvironment::EPFFS_Water)) == true)
	)
	{
		CurrentEnvironment_ = EPFFlowerEnvironment::EPFFS_Water;
	} 
	else
	{
		CurrentEnvironment_ = EPFFlowerEnvironment::EPFFS_None;
		CurrentFlowerClassToSpawn_ = nullptr;
		return;
	}
	
	CurrentFlowerClassToSpawn_ = GetRandomClassToSpawnAccordingToEnvironment(CurrentEnvironment_);
}

float UPFFlowerSpawnerResource::DetermineSpawnDelay()
{
	float DelayAtMinimalVelocity;
	float DelayAtMaximalVelocity;
	
	switch (CurrentEnvironment_)
	{
		case EPFFlowerEnvironment::EPFFS_Landscape:
			DelayAtMinimalVelocity = DataPtr_->DelayBetweenTwoFlowerSpawnsAtMinimalVelocity;
			DelayAtMaximalVelocity = DataPtr_->DelayBetweenTwoFlowerSpawnsAtMaximalVelocity;
			break;
		case EPFFlowerEnvironment::EPFFS_Water:
			DelayAtMinimalVelocity = DataPtr_->DelayBetweenTwoWaterLilySpawnsAtMinimalVelocity;
			DelayAtMaximalVelocity = DataPtr_->DelayBetweenTwoWaterLilySpawnsAtMaximalVelocity;
			break;
		case EPFFlowerEnvironment::EPFFS_Cliff:
			DelayAtMinimalVelocity = DataPtr_->DelayBetweenTwoIvySpawnsAtMinimalVelocity;
			DelayAtMaximalVelocity = DataPtr_->DelayBetweenTwoIvySpawnsAtMaximalVelocity;
			break;
		case EPFFlowerEnvironment::EPFFS_None:
		default:
			DelayAtMinimalVelocity = 1.f;
			DelayAtMaximalVelocity = 1.f;
			break;
	}
	float PlayerVelocityPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
	// float PlayerVelocityPercentage = 0.f;

	float SpawnDelay = FMath::Lerp(DelayAtMinimalVelocity, DelayAtMaximalVelocity, PlayerVelocityPercentage);

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
	
	TArray<FHitResult> NewHitResults;
	OwnerWorldPtr_->SweepMultiByObjectType(NewHitResults, PlayerPosition, PlayerPosition + LengthenVector,
										FQuat::Identity, CachedObjectQueryParams, sphere, CachedQueryParams);
	
	FHitResult FinalHitResult;
	for(const FHitResult& NewHitResult: NewHitResults)
	{
		if(!CheckSpawnConditions(NewHitResult)) continue;
		
		FinalHitResult = NewHitResult;
		break;
	}
	
	if (FinalHitResult.GetActor() == nullptr) return;
	SetCurrentClassToSpawn(FinalHitResult);
	if (CurrentFlowerClassToSpawn_ == nullptr) return;
	
	float FlowerHeight = GetRandomFlowerHeight(FinalHitResult.ImpactPoint.Z);
	FVector SpawnLocation = FVector(FinalHitResult.ImpactPoint.X, FinalHitResult.ImpactPoint.Y, FlowerHeight);

	// Rotation de la fleur :
	FVector SafeNormal = FinalHitResult.ImpactNormal.GetSafeNormal();
	if (SafeNormal.IsNearlyZero()) SafeNormal = FVector::UpVector;
	FQuat AlignmentQuat = FRotationMatrix::MakeFromZX(SafeNormal, FVector::ForwardVector).ToQuat();
	float RandomRotation = FMath::RandRange(0, 360);
	FQuat FinalQuat = AlignmentQuat * FQuat(FVector::UpVector, FMath::DegreesToRadians(RandomRotation));
	FRotator FinalRotation = FinalQuat.Rotator();
	
	// Spawn avec PoolSystem : 	
	APFFlower* Flower = Cast<APFFlower>(PoolSubsystemPtr_->SpawnFromPool<AActor>(CurrentFlowerClassToSpawn_, SpawnLocation, FinalRotation, GetRandomFlowerSize()));

	// Change Actor Color
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(0, CurrentColorValue_.R);
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(1, CurrentColorValue_.G);
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(2, CurrentColorValue_.B);
	// color activation via Custom primitive data :
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(7, 1);
	int isFlower;
	if (CurrentEnvironment_ == EPFFlowerEnvironment::EPFFS_Water)
	{
		isFlower = 0;
	} else
	{
		isFlower = 1;
	}
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(8, isFlower);
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
	
	if (!OwnerWorldPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The OwnerWorldPtr_ is NULL"));
		return false;
	}

	if (!ProximityResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The ProximityResourcePtr_ referenced in FlowerSpawnerResource blueprint is NULL"))
		return false;
	}
	
	if (!PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The PhysicResourcePtr is NULL"));
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
	
	if (!PoolSubsystemPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFFlowerSpawnerResource] The PoolSubsystemPtr_ is NULL"))
		return false;
	}
	
	return true;
}	

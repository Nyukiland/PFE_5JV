// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource/PFFlowerSpawnerResource.h"

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

	// Pour chaque type d'acteurs qui a un pool, si on a atteint la limite d'acteurs placés, on remplace par la version HISM :
	for (const auto& Pair : PoolSubsystemPtr_->ObjectPools)
	{
		UClass* PoolClass = Pair.Key;
		FPFPoolArrays ObjectPool = Pair.Value;
		
		if(ObjectPool.PlacedObjectsNum() >= MaxActorsAmountPlaced) {
			OnActorsByHismSwitchDelegate.Broadcast(PoolClass, CurrentColorValue, ObjectPool.PlacedObjectTransforms);
			PoolSubsystemPtr_->ReturnToPool(PoolClass);
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

		if(ObjectPool.PlacedObjectsIsEmpty() == false)
		{
			OnActorsByHismSwitchDelegate.Broadcast(PoolClass, CurrentColorValue, ObjectPool.PlacedObjectTransforms);
		}
	}
	
	// Implement new color :
	CurrentFlowerColor_ = FlowerColor;
	UFlowerSpawnerHelper::TryGetFlowerColorFromEnum(CurrentFlowerColor_, CurrentColorValue);
	OnFlowerColorChangeDelegate.Broadcast(CurrentFlowerColor_);
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

TSubclassOf<APFFlower> UPFFlowerSpawnerResource::GetRandomClassToSpawnAccordingToEnvironment(EPFFlowerEnvironment Environment)
{
	if (Environment ==  EPFFlowerEnvironment::EPFFS_Water) FString EnvironmentString = UFlowerSpawnerHelper::GetFlowerEnvironmentNameFromEnum(Environment).ToString();
	
	FPFEnvironmentFlowers* EnvironmentFlowers = FlowersByEnvironments.Find(Environment);
	
	// if (Environment ==  EPFFlowerEnvironment::EPFFS_Water)
	// {
	// 	if (EnvironmentFlowers) UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] struct EnvironmentFlowers pour Eau trouvé"));
	// 	if (!EnvironmentFlowers) UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] struct EnvironmentFlowers  pour Eau NON trouvé"));
	// }

	TArray<TSubclassOf<APFFlower>> FlowerClasses = EnvironmentFlowers->FlowerClasses;
	
	// if (Environment ==  EPFFlowerEnvironment::EPFFS_Water)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] %d Classes pour l'environnement Eau Trouvé"), FlowerClasses.Num());
	// }
	
	int MaxRange = FlowerClasses.Num() - 1;
	
	// if (Environment ==  EPFFlowerEnvironment::EPFFS_Water)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] MaxRange pour l'environnement Eau = %d"), MaxRange);
	// }
	int RandomFlowerClassIndex = FMath::RandRange(0, MaxRange);
	// if (Environment ==  EPFFlowerEnvironment::EPFFS_Water)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] RandomFlowerClassIndex pour l'environnement Eau = %d"), RandomFlowerClassIndex);
	// 	TSubclassOf<APFFlower> Class = FlowerClasses[RandomFlowerClassIndex];
	// 	FString ClassString = Class->GetName();
	// 	UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] Class pour l'eau : %s"), *ClassString);
	// }
	
	
	return FlowerClasses[RandomFlowerClassIndex];
}

bool UPFFlowerSpawnerResource::CheckSpawnConditions(const FHitResult& Hit)
{
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] CheckSpawnConditions : Début"));
	} 
	if (!Hit.bBlockingHit) return false;
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] CheckSpawnConditions : bBlockingHit FALSE"));
	}
	
	if (!Hit.GetActor()) return false;
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] CheckSpawnConditions : On a un acteur"));
	} 
	
	return true;
}

void UPFFlowerSpawnerResource::SetCurrentClassToSpawn(const FHitResult& Hit)
{
	// Détermine le type de class qu'on va spawn  : 
	// UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] %s"), *Hit.GetActor()->GetName());
	EPFFlowerEnvironment Environment;
	
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
			Environment = EPFFlowerEnvironment::EPFFS_Cliff;
		} 
		else
		{
			Environment = EPFFlowerEnvironment::EPFFS_Landscape;
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
		Environment = EPFFlowerEnvironment::EPFFS_Water;
		CurrentFlowerClassToSpawn = GetRandomClassToSpawnAccordingToEnvironment(Environment);
		UE_LOG(LogTemp, Warning, TEXT("[FlowerSpaner] Spawn On Water : Class = %s"), *LexToString(CurrentFlowerClassToSpawn->GetName()));
	} 
	else
	{
		CurrentFlowerClassToSpawn = nullptr;
		return;
	}
	
	CurrentFlowerClassToSpawn = GetRandomClassToSpawnAccordingToEnvironment(Environment);
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
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource]  nénuphars 1"));
	} 
	TArray<FHitResult> ValidHitResults = ProximityResourcePtr_->ValidHitResults;
	if(ValidHitResults.IsEmpty()) return;

	FHitResult InitialHitResult;
	
	for(const FHitResult& ValidHitResult: ValidHitResults)
	{
		if(!CheckSpawnConditions(ValidHitResult)) continue;
		
		InitialHitResult = ValidHitResult;
		break;
	}

	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource]  nénuphars 2"));
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
	// OwnerWorldPtr_->SweepSingleByChannel(CurrentHitResult, PlayerPosition, PlayerPosition + LengthenVector,
	// 								FQuat::Identity, ECC_Visibility, sphere, queryParams);
	
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
	
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource]  nénuphars 3"));
	} 
	// if(!CheckSpawnConditions(CurrentHitResult)) return;
	SetCurrentClassToSpawn(FinalHitResult);
	if (CurrentFlowerClassToSpawn == nullptr) return;
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource]  nénuphars 4"));
	} 
	float FlowerHeight = GetRandomFlowerHeight(CurrentHitResult.ImpactPoint.Z);
	FVector SpawnLocation = FVector(CurrentHitResult.ImpactPoint.X, CurrentHitResult.ImpactPoint.Y, FlowerHeight);

	// Rotation de la fleur :
	FQuat AlignmentQuat = FRotationMatrix::MakeFromZX(CurrentHitResult.ImpactNormal, FVector::ForwardVector).ToQuat();
	float RandomRotation = FMath::RandRange(0, 360);
	FQuat FinalQuat = AlignmentQuat * FQuat(FVector::UpVector, FMath::DegreesToRadians(RandomRotation));
	FRotator FinalRotation = FinalQuat.Rotator();

	// Spawn avec PoolSystem : 
	if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] On spawn des nénuphars !!!!"));
	} else if (CurrentFlowerClassToSpawn != nullptr and CurrentFlowerClassToSpawn->GetName() == "BP_Ivy_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource] On spawn du lierre ..."));
	}
	
	APFFlower* Flower = Cast<APFFlower>(PoolSubsystemPtr_->SpawnFromPool<AActor>(CurrentFlowerClassToSpawn, SpawnLocation, FinalRotation));
	if (CurrentFlowerClassToSpawn->GetName() == "BP_WaterLily_C")
	{
		UE_LOG(LogTemp, Log, TEXT("[FlowerSpawnerResource]  nénuphars 5"));
	} 
	// Change Size :
	FVector FlowerSize = GetRandomFlowerSize();
	Flower->SetActorScale3D(FlowerSize);

	// Change Actor Color
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(0, CurrentColorValue.R);
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(1, CurrentColorValue.G);
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(2, CurrentColorValue.B);
	// color activation via Custom primitive data :
	Flower->GetFlowerMesh()->SetCustomPrimitiveDataFloat(7, 1);
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

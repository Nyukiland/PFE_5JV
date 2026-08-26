#include "Helpers/PFPainter.h"

#include "EngineUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Helpers/FlowerSpawner/FlowerSpawnerHelper.h"
#include "Helpers/FlowerSpawner/PFFlowerSpawnerTypes.h"

APFPainter* APFPainter::Instance = nullptr;

APFPainter::APFPainter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APFPainter::BeginPlay()
{
	Super::BeginPlay();
	for(const auto Pair : StaticMeshModelsToSpawn)
	{
		UClass* FlowerClass = Pair.Key;
		CreateNewHismModel(FlowerClass);
	}

	OnMaxHismAmountInstancedDelegate.AddUObject(this, &APFPainter::CreateNewHismModel);
}

void APFPainter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for(const auto& Pair : StaticMeshModelsToSpawn)
	{
		UClass* ActorClass = Pair.Key;
		FPFStaticMeshModelData CurrentHismData = Pair.Value;
		if(CurrentHismData.ActiveModelHismPtr_->GetInstanceCount() >= MaxHismIntances) OnMaxHismAmountInstancedDelegate.Broadcast(ActorClass);
	}
}

APFPainter* APFPainter::GetPainter(UObject* WorldContext)
{
	if (!Instance || !IsValid(Instance))
	{
		UWorld* World = WorldContext ? WorldContext->GetWorld() : nullptr;
		if (!World) 
		{
			UE_LOG(LogTemp, Error, TEXT("[Painter] No valid World Context provided."));
			return nullptr;
		}

		for (TActorIterator<APFPainter> It(World); It; ++It)
		{
			if (*It && IsValid(*It))
			{
				Instance = *It;
				return Instance;
			}
		}
		
		UE_LOG(LogTemp, Error, TEXT("[Painter] The instance of painter has not been placed on the scene"));
		
		if (!Instance)
		{
			UE_LOG(LogTemp, Error, TEXT("[Painter] Failed to spawn new instance."));
		}
	}
	
	return Instance;
}

void APFPainter::CreateNewHismModel(const TSubclassOf<AActor>& ActorClass)
{
	if(ActorClass == nullptr) return;
	FPFStaticMeshModelData& Hism = StaticMeshModelsToSpawn.FindOrAdd(ActorClass);
	Hism.ActiveModelIndex++;

	// Create the new HISM :
	FString ClassName = ActorClass->GetName();
	ClassName = ClassName.Replace(TEXT("BP_"), TEXT("HISM_"));
	ClassName += FString::Printf(TEXT("_%d"), Hism.ActiveModelIndex);
	UE_LOG(LogTemp, Warning, TEXT("[Painter] Création d'un nouveau modèle d'HISM : %s"), *ClassName);
	UHierarchicalInstancedStaticMeshComponent* NewHism = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, FName(*ClassName));
	if(NewHism == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Painter] New HISM %s creation failed"), *ClassName);
		return;
	}

	// Make the new HISM visible the component in the detail pannel of the outliner :
	NewHism->CreationMethod = EComponentCreationMethod::Instance;
	NewHism->RegisterComponent();
	NewHism->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	this->AddInstanceComponent(NewHism);
		
	Hism.ActiveModelHismPtr_ = NewHism;

	// Initialize the new HISM :
	InitializeHism(ActorClass, Hism.ActiveModelHismPtr_);
}

void APFPainter::InitializeHism(const TSubclassOf<AActor>& ActorClass, UHierarchicalInstancedStaticMeshComponent* HismPtr_)
{
	if(HismPtr_ == nullptr) return;
	HismPtr_->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HismPtr_->SetNumCustomDataFloats(3);
	if(ActorClass == nullptr) return;
	if(AActor* DefaultActor = ActorClass->GetDefaultObject<AActor>())
	{
		if(UStaticMeshComponent* MeshComponent = DefaultActor->FindComponentByClass<UStaticMeshComponent>())
		{
			if(UStaticMesh* Mesh = MeshComponent->GetStaticMesh())
			{
				HismPtr_->SetStaticMesh(Mesh);
			}
			
			if(UMaterialInterface* Material = MeshComponent->GetMaterial(0))
			{
				HismPtr_->SetMaterial(0, Material);
			}
		}
	}
	
	// Spawn a first HISM instance to avoid spike later
	FTransform Transform = FTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, -2000.f), FVector::ZeroVector);
	HismPtr_->AddInstance(Transform, true);
}

void APFPainter::ReplaceActorsByHismByClass(const TSubclassOf<AActor>& ActorClass, const FLinearColor ColorValue, const TArray<FTransform>& PlacedObjectTransforms)
{
	UE_LOG(LogTemp, Warning,TEXT( "[Painter] ReplaceActorsByHismByClass"));
	FPFStaticMeshModelData* CurrentHismData = StaticMeshModelsToSpawn.Find(ActorClass);
	if(CurrentHismData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Painter] There is no HISM model for %s class. Add it in HismToGenerate array"), *ActorClass->GetName());
		return;
	}
	UHierarchicalInstancedStaticMeshComponent* CurrentHismPtr_ = CurrentHismData->ActiveModelHismPtr_;
	TArray<int32> Indices = CurrentHismPtr_->AddInstances(PlacedObjectTransforms, true, true, false);

	for(int32 InstanceIndex : Indices)
	{
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 0, ColorValue.R, false);
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 1, ColorValue.G, false);
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 2, ColorValue.B, false);
	}
	CurrentHismPtr_->MarkRenderStateDirty();
}

void APFPainter::PaintStuff_Implementation(const TArray<FHitResult>& validHitResults, const TArray<float>& brushSizes)
{
}

void APFPainter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (Instance == this)
	{
		Instance = nullptr;
	}
}


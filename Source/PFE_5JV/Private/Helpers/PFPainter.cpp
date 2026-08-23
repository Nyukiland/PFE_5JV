#include "Helpers/PFPainter.h"

#include "EngineUtils.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"

APFPainter* APFPainter::Instance = nullptr;

APFPainter::APFPainter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APFPainter::CreateNewHismModel(const TSubclassOf<AActor>& ActorClass)
{
	if(ActorClass == nullptr) return;
	FPFHismData& Hism = ActiveHisms.FindOrAdd(ActorClass);
	Hism.Index++;

	// Create the new HISM :
	FString ClassName = ActorClass->GetName();
	ClassName = ClassName.Replace(TEXT("BP_"), TEXT("HISM_"));
	ClassName += FString::Printf(TEXT("_%d"), Hism.Index);
	UE_LOG(LogTemp, Warning, TEXT("ClassName : %s"), *ClassName);
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
		
	Hism.HismPtr_ = NewHism;

	// Initialize the new HISM :
	InitializeHism(ActorClass, Hism.HismPtr_);
}

void APFPainter::InitializeHism(const TSubclassOf<AActor>& ActorClass, UHierarchicalInstancedStaticMeshComponent* HismPtr_)
{
	if(ActorClass == nullptr) return;
	AActor* DefaultActor = ActorClass->GetDefaultObject<AActor>();
	if(DefaultActor == nullptr) return;
	UStaticMeshComponent* MeshComponent = DefaultActor->FindComponentByClass<UStaticMeshComponent>();
	if(MeshComponent == nullptr) return;
	UStaticMesh* Mesh = MeshComponent->GetStaticMesh();
	if(Mesh == nullptr) return;
	UMaterialInterface* Material = Mesh->GetMaterial(0);
	if(Material == nullptr) return;
	HismPtr_->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HismPtr_->SetStaticMesh(Mesh);
	HismPtr_->SetMaterial(0, Material);
	HismPtr_->SetNumCustomDataFloats(3);
}



void APFPainter::BeginPlay()
{
	Super::BeginPlay();
	for(UClass* Class : HismToGenerate)
	{
		CreateNewHismModel(Class);
	}
}

void APFPainter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

// TODO : changer rendre les paramètres const et passer par référence
void APFPainter::ReplaceActorsByHismByClass(TSubclassOf<AActor> ActorClass, FLinearColor ColorValue, TArray<FTransform> PlacedObjectTransforms)
{
	FPFHismData* CurrentHismData = ActiveHisms.Find(ActorClass);
	if(CurrentHismData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[Painter] There is no HISM model for %s class. Add it in HismToGenerate array"), *ActorClass->GetName());
		return;
	}
	UHierarchicalInstancedStaticMeshComponent* CurrentHismPtr_ = CurrentHismData->HismPtr_;
	TArray<int32> Indices = CurrentHismPtr_->AddInstances(PlacedObjectTransforms, true, true, false);

	for(int32 InstanceIndex : Indices)
	{
		// UE_LOG(LogTemp, Error, TEXT("[Painter] HISM Indice  : %d"), InstanceIndex);
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 0, ColorValue.R, true);
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 1, ColorValue.G, true);
		CurrentHismPtr_->SetCustomDataValue(InstanceIndex, 2, ColorValue.B, true);
		// Active la couleur via HISM
		CurrentHismPtr_->SetCustomPrimitiveDataFloat(7, 1.0f);
	}
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


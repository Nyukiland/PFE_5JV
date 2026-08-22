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
	FPFHismData& Hism = CurrentHism.FindOrAdd(ActorClass);
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
	GetHismInitializationDataFromClass(ActorClass, Hism.HismPtr_);
}

void APFPainter::GetHismInitializationDataFromClass(const TSubclassOf<AActor>& ActorClass, UHierarchicalInstancedStaticMeshComponent* HismPtr_)
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

	HismPtr_->SetStaticMesh(Mesh);
	HismPtr_->SetMaterial(0, Material);
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


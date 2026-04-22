#include "Helpers/PFPainter.h"

#include "EngineUtils.h"

APFPainter* APFPainter::Instance = nullptr;

APFPainter::APFPainter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APFPainter::BeginPlay()
{
	Super::BeginPlay();
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

void APFPainter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	if (Instance == this)
	{
		Instance = nullptr;
	}
}

void APFPainter::PaintStuff_Implementation(FVector start, FVector end,
	float BrushSphereDistanceRef, FVector2D BrushSizeMinMaxRef, float BrushDistance)
{
}
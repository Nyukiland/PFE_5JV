#include "PFKuwaharaRendererSubsystem.h"

#include "PFKuwaharaRendererSubsystem.h"
#include "PFKuwaharaSettings.h"
#include "Engine/Texture2D.h"

void UPFKuwaharaRendererSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UPFKuwaharaSettings* Settings = GetDefault<UPFKuwaharaSettings>();
	if (Settings && !Settings->BlueNoiseTexture.IsNull())
	{
		LoadedBlueNoise = Settings->BlueNoiseTexture.LoadSynchronous();
	}

	KuwaharaSVE = FSceneViewExtensions::NewExtension<PFKuwaharaViewExtension>(LoadedBlueNoise);
}

void UPFKuwaharaRendererSubsystem::Deinitialize()
{
	KuwaharaSVE.Reset();
	LoadedBlueNoise = nullptr;
	Super::Deinitialize();
}

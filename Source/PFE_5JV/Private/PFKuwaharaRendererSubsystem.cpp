#include "PFKuwaharaRendererSubsystem.h"

#include "PFKuwaharaSettings.h"
#include "SceneViewExtension.h"

void UPFKuwaharaRendererSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UPFKuwaharaSettings* Settings = GetDefault<UPFKuwaharaSettings>();
	UTexture2D* LoadedNoise = Settings->BlueNoiseTexture.LoadSynchronous();
    
	FTextureRHIRef NoiseRHI = nullptr;
	if (LoadedNoise && LoadedNoise->GetResource())
	{
		NoiseRHI = LoadedNoise->GetResource()->GetTexture2DRHI();
	}
    
	KuwaharaSVE = FSceneViewExtensions::NewExtension<PFKuwaharaViewExtension>(NoiseRHI);
}

void UPFKuwaharaRendererSubsystem::Deinitialize()
{
	if (KuwaharaSVE.IsValid())
	{
		KuwaharaSVE.Reset();
	}
	
	Super::Deinitialize();
}

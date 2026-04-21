#include "PFGameAdditionalShutdownSystem.h"

#include "Misc/CoreDelegates.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "FMODStudioModule.h"
#include "fmod_studio.hpp"

void UPFGameAdditionalShutdownSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	TerminateDelegateHandle = FCoreDelegates::GetApplicationWillTerminateDelegate()
		.AddUObject(this, &UPFGameAdditionalShutdownSystem::OnApplicationEnd);
}

void UPFGameAdditionalShutdownSystem::Deinitialize()
{
	if (TerminateDelegateHandle.IsValid())
	{
		FCoreDelegates::GetApplicationWillTerminateDelegate().Remove(TerminateDelegateHandle);
	}
	
	Super::Deinitialize();
}

void UPFGameAdditionalShutdownSystem::OnApplicationEnd()
{
	if (IFMODStudioModule::IsAvailable())
	{
		FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
		if (StudioSystem)
		{
			FMOD::System* CoreSystem = nullptr;
			StudioSystem->getCoreSystem(&CoreSystem);
            
			if (CoreSystem)
			{
				CoreSystem->mixerSuspend();
			}
		}
	}

	FGenericPlatformMisc::RequestExit(true);
}

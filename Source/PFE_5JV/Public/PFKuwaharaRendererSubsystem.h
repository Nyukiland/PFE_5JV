#pragma once

#include "CoreMinimal.h"
#include "PFKuwaharaViewExtension.h"
#include "Subsystems/EngineSubsystem.h"
#include "PFKuwaharaRendererSubsystem.generated.h"


UCLASS()
class PFE_5JV_API UPFKuwaharaRendererSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
    
private:
	TSharedPtr<PFKuwaharaViewExtension, ESPMode::ThreadSafe> KuwaharaSVE;
};

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PFGameAdditionalShutdownSystem.generated.h"

UCLASS()
class PFE_5JV_API UPFGameAdditionalShutdownSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
private:
	void OnApplicationEnd();	
	
	FDelegateHandle TerminateDelegateHandle;
};

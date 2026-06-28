#pragma once

#include "CoreMinimal.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/State/PFState.h"
#include "PFPauseState.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFPauseState : public UPFState
{
	GENERATED_BODY()
	
	private:
	UPROPERTY()
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;
	
	public:
	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;
};

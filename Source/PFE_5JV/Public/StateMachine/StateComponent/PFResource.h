#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFStateComponent.h"
#include "PFResource.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFResource : public UPFStateComponent
{
	GENERATED_BODY()

	public:
	virtual int GetPriority() const override;
};

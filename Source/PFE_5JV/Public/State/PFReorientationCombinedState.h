#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFCombinedState.h"
#include "PFReorientationCombinedState.generated.h"

UCLASS()
class PFE_5JV_API UPFReorientationCombinedState : public UPFCombinedState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;
};

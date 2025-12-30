#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PFState.generated.h"

UCLASS(Blueprintable)
class PFE_5JV_API UPFState : public UObject
{
	GENERATED_BODY()

public:
	void OnEnter();
	void OnTick(float deltaTime);
	void OnExit();
};

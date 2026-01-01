#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PFState.generated.h"

UCLASS(Blueprintable)
class PFE_5JV_API UPFState : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnEnter();
	virtual void OnEnter_Implementation();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTick(float deltaTime);
	virtual void OnTick_Implementation(float deltaTime);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnExit();
	virtual void OnExit_Implementation();
};

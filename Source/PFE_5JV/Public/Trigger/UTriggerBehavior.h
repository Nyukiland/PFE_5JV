#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UTriggerBehavior.generated.h"


UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UUTriggerBehavior : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUTriggerBehavior();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTriggerEnter();
	virtual void OnTriggerEnter_Implementation();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTriggerExit();
	virtual void OnTriggerExit_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTriggerTick(float deltaTime);
	virtual void OnTriggerTick_Implementation(float deltaTime);
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PFTriggerBehavior.generated.h"


class APFTrigger;

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFTriggerBehavior : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(visibleAnywhere, BlueprintReadOnly)
	TObjectPtr<APFTrigger> TriggerObjectPtr_;
	
public:	
	UPFTriggerBehavior();

	void Init(APFTrigger* parentTrigger);
	
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

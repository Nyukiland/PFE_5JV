#pragma once

#include "CoreMinimal.h"
#include "StateMachine/State/PFState.h"
#include "PFCombinedState.generated.h"

UCLASS(Abstract, blueprintable)
class PFE_5JV_API UPFCombinedState : public UPFState
{
	GENERATED_BODY()

	protected:
	UPROPERTY()
	TArray<TObjectPtr<UPFState>> SubStatePtr_;
	
	public:
	virtual void OnTick_Implementation(float deltaTime) override;

	virtual void OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent, const FInputActionValue& value) override;

	virtual void OnExit_Implementation() override;

	UFUNCTION(blueprintable)
	void AddSubState(TSubclassOf<UPFState> stateClass);
};

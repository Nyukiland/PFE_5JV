#include "StateMachine/State/PFCombinedState.h"

void UPFCombinedState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	for (UPFState* state : SubStatePtr_)
	{
		if (state)
			state->OnTick(deltaTime);
	}
}

void UPFCombinedState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	for (UPFState* state : SubStatePtr_)
	{
		if (state)
			state->OnInputTriggered(inputName, triggerEvent, value);
	}
}

void UPFCombinedState::OnExit_Implementation()
{
	Super::OnExit_Implementation();
	
	for (UPFState* state : SubStatePtr_)
	{
		if (state)
			state->OnExit();
	}
}

void UPFCombinedState::AddSubState(TSubclassOf<UPFState> stateClass)
{
	if (!stateClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[CombinedState] AddSubstate subclass is null"));
		return;
	}
	
	UPFState* state = NewObject<UPFState>(this, stateClass);

	if (!state)
	{
		UE_LOG(LogTemp, Error, TEXT("[CombinedState] AddSubstate issue with creating class %s"), *stateClass->GetName());
		return;
	}
	
	state->Owner = Owner;
	state->OnEnter();

	SubStatePtr_.Add(state);
}

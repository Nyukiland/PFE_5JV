#include "StateMachine/State/PFCombinedState.h"

void UPFCombinedState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	for (UPFState* state : SubStatePtr_)
		state->OnEnter();
}

void UPFCombinedState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	for (UPFState* state : SubStatePtr_)
		state->OnInputTriggered(inputName, triggerEvent, value);
}

void UPFCombinedState::OnExit_Implementation()
{
	Super::OnExit_Implementation();
	
	for (UPFState* state : SubStatePtr_)
		state->OnExit();
}

void UPFCombinedState::AddSubState(TSubclassOf<UPFState> stateClass)
{
	UPFState* state = NewObject<UPFState>(this, stateClass);
	state->OnEnter();

	SubStatePtr_.Add(state);
}

#include "State/PFTurnSubState.h"

#include "Ability/PFTurnAbility.h"
#include "Helpers/PFBlueprintHelper.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFTurnSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	TurnPtr_ = Owner->GetAndActivateComponent<UPFTurnAbility>();
}

void UPFTurnSubState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	if (!TurnPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	EPlayerInput input;
	UPFBlueprintHelper::FilterAllInputRaw(inputName, input);

	if (input == EPlayerInput::LeftTrigger)
	{
		TurnPtr_->ReceiveInputLeft(value.Get<float>());
		return;
	}

	if (input == EPlayerInput::RightTrigger)
	{
		TurnPtr_->ReceiveInputRight(value.Get<float>());
	}
}

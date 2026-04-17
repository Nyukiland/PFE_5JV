#include "State/PFWingBeatSubState.h"

#include "Ability/PFWingBeatAbility.h"
#include "Helpers/PFBlueprintHelper.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFWingBeatSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	WingBeatPtr_ = Owner->GetAndActivateComponent<UPFWingBeatAbility>();
}

void UPFWingBeatSubState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	if (!WingBeatPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatSubState] Bad set up"));
		return;	
	}

	EExecPinsResult result;
	UPFBlueprintHelper::FilterSpecificInputWithTrigger(EPlayerInput::WingBeat, ETriggerEvent::Triggered,
		inputName, triggerEvent, result);

	if (result == EExecPinsResult::Passed)
	{
		WingBeatPtr_->WingBeat();
	}
}

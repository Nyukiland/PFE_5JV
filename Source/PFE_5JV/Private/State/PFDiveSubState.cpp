#include "State/PFDiveSubState.h"

#include "Ability/PFDiveAbility.h"
#include "Helpers/PFBlueprintHelper.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFDiveSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = Owner->GetAndActivateComponent<UPFDiveAbility>();
}

void UPFDiveSubState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	if (!DivePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveSubState] Bad set up"));
		return;	
	}
	
	EPlayerInput input;
	UPFBlueprintHelper::FilterAllInputRaw(inputName, input);

	if (input == EPlayerInput::LeftTrigger)
	{
		DivePtr_->ReceiveInputLeft(value.Get<float>());
		return;
	}

	if (input == EPlayerInput::RightTrigger)
	{
		DivePtr_->ReceiveInputRight(value.Get<float>());
	}
}

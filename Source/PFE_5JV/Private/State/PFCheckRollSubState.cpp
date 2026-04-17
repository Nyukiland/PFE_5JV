#include "State/PFCheckRollSubState.h"

#include "Ability/PFRollAbility.h"
#include "Helpers/PFBlueprintHelper.h"
#include "State/PFRollState.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCheckRollSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = Owner->GetStateComponent<UPFRollAbility>();
}

void UPFCheckRollSubState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	if (!RollPtr_ || !RollState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CheckRoll] Bad set up"));
		return;
	}

	if (RollPtr_->PreRollCheck(deltaTime))
	{
		Owner->ChangeState(RollState_);
	}
	
}

void UPFCheckRollSubState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);
	
	if (!RollPtr_ || !RollState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CheckRoll] Bad set up"));
		return;
	}

	EExecPinsResult result;
	UPFBlueprintHelper::FilterSpecificInputRaw(EPlayerInput::LeftTrigger, inputName, result);

	if (result == EExecPinsResult::Passed)
	{
		RollPtr_->TryCallRollLeft(value.Get<float>(), -1);
		return;
	}

	UPFBlueprintHelper::FilterSpecificInputRaw(EPlayerInput::RightTrigger, inputName, result);

	if (result == EExecPinsResult::Passed)
	{
		RollPtr_->TryCallRollRight(value.Get<float>(), 1);
	}
}

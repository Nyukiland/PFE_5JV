#include "State/PFBaseCombinedState.h"

#include "Helpers/PFBlueprintHelper.h"
#include "State/PFAutoDiveState.h"
#include "State/PFCheckRollSubState.h"
#include "State/PFDiveSubState.h"
#include "State/PFTurnSubState.h"
#include "State/PFWingBeatSubState.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFBaseCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	GlidePtr_ = Owner->GetAndActivateComponent<UPFGlideAbility>();

	if (!CheckRollSubState_ || !TurnSubState_
		|| !DiveSubState_ || !WingBeatSubState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	AddSubState(CheckRollSubState_);
	AddSubState(TurnSubState_);
	AddSubState(DiveSubState_);
	AddSubState(WingBeatSubState_);
}

void UPFBaseCombinedState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	if (GlidePtr_ && GlidePtr_->ShouldGoInAutoDive())
	{
		Owner->ChangeState(AutoDiveState_);
	}
}

void UPFBaseCombinedState::OnInputTriggered_Implementation(FName inputName, ETriggerEvent triggerEvent,
	const FInputActionValue& value)
{
	Super::OnInputTriggered_Implementation(inputName, triggerEvent, value);

	if (!GlidePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	EExecPinsResult result;
	UPFBlueprintHelper::FilterSpecificInputRaw(EPlayerInput::Move, inputName, result);

	if (result == EExecPinsResult::Passed)
	{
		FVector2D moveInput = value.Get<FVector2D>();
		GlidePtr_->ReceiveInputMovement(moveInput);
	}
}

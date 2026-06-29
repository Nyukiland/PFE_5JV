#include "State/PFPauseState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFPauseState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();
	
	PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	HapticResourcePtr_ = Owner->GetStateComponent<UPFHapticsResource>();
	HapticResourcePtr_->AskToPauseHaptics();
	PhysicResourcePtr_->SetKinematic(true);
	Owner->SetStateMachineActiveState(false);
}

void UPFPauseState::OnExit_Implementation()
{
	Super::OnExit_Implementation();
	
	HapticResourcePtr_->AskToResumeHaptics();
	PhysicResourcePtr_->SetKinematic(false);
	Owner->SetStateMachineActiveState(true);
}

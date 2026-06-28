#include "State/PFPauseState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFPauseState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();
	
	PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	PhysicResourcePtr_->SetKinematic(true);
	Owner->SetStateMachineActiveState(false);
}

void UPFPauseState::OnExit_Implementation()
{
	Super::OnExit_Implementation();
	
	PhysicResourcePtr_->SetKinematic(false);
	Owner->SetStateMachineActiveState(true);
}

#include "State/PFBaseCombinedState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFBaseCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	GlidePtr_ = Owner->GetAndActivateComponent<UPFGlideAbility>();
}
#include "State/PFRollState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFRollState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = Owner->GetAndActivateComponent<UPFRollAbility>();
}

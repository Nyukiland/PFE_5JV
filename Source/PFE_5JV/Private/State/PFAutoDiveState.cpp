#include "State/PFAutoDiveState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFAutoDiveState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = Owner->GetAndActivateComponent<UPFDiveAbility>();
	WingBeatPtr_ = Owner->GetAndActivateComponent<UPFWingBeatAbility>();
}

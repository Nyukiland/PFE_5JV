#include "State/PFWingBeatSubState.h"

#include "Ability/PFWingBeatAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFWingBeatSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	WingBeatPtr_ = Owner->GetAndActivateComponent<UPFWingBeatAbility>();
}

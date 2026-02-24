#include "State/PFDiveState.h"

#include "Ability/PFDiveAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFDiveState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = Owner->GetAndActivateComponent<UPFDiveAbility>();
}

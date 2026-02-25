#include "State/PFDiveSubState.h"

#include "Ability/PFDiveAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFDiveSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = Owner->GetAndActivateComponent<UPFDiveAbility>();
}

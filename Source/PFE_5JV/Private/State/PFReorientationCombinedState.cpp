#include "State/PFReorientationCombinedState.h"

#include "Ability/PFGlideAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFReorientationCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	Owner->GetAndActivateComponent<UPFGlideAbility>();
}

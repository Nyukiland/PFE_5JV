#include "State/PFTurnSubState.h"

#include "Ability/PFTurnAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFTurnSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	TurnPtr_ = Owner->GetAndActivateComponent<UPFTurnAbility>();
}

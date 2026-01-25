#include "State/PFTurnState.h"

#include "Ability/PFTurnAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFTurnState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	TurnPtr_ = CastChecked<UPFTurnAbility>(
		Owner->GetAndActivateComponent(UPFTurnAbility::StaticClass()));
}

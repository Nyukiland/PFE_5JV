#include "State/PFCheckRollSubState.h"

#include "Ability/PFRollAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCheckRollSubState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = Owner->GetStateComponent<UPFRollAbility>();
}

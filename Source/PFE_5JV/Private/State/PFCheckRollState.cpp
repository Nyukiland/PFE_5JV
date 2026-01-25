#include "State/PFCheckRollState.h"

#include "Ability/PFRollAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCheckRollState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = CastChecked<UPFRollAbility>(
		Owner->GetStateComponent(UPFRollAbility::StaticClass()));
}

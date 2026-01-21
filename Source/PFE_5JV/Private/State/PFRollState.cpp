#include "State/PFRollState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFRollState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	Roll_ = CastChecked<UPFRollAbility>(
		Owner->GetAndActivateComponent(UPFRollAbility::StaticClass()));
}

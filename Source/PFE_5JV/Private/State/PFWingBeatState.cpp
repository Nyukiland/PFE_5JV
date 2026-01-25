#include "State/PFWingBeatState.h"

#include "Ability/PFWingBeatAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFWingBeatState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	WingBeatPtr_ = CastChecked<UPFWingBeatAbility>(
	Owner->GetAndActivateComponent(UPFWingBeatAbility::StaticClass()));
}

#include "State/PFAutoDiveState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFAutoDiveState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = CastChecked<UPFDiveAbility>(
		Owner->GetAndActivateComponent(UPFDiveAbility::StaticClass()));

	WingBeatPtr_ = CastChecked<UPFWingBeatAbility>(
		Owner->GetAndActivateComponent(UPFWingBeatAbility::StaticClass()));
}

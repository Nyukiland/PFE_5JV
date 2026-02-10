#include "State/PFRollState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFRollState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = CastChecked<UPFRollAbility>(
		Owner->GetAndActivateComponent(UPFRollAbility::StaticClass()));
}

void UPFRollState::RollCheckInputLeft(float inputValue)
{
	
}

void UPFRollState::RollCheckInputRight(float inputValue)
{
	
}

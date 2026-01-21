#include "State/PFBaseCombinedState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFBaseCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	Glide_ = CastChecked<UPFGlideAbility>(
		Owner->GetAndActivateComponent(UPFGlideAbility::StaticClass()));
	
	Dive_ = CastChecked<UPFDiveAbility>(
		Owner->GetAndActivateComponent(UPFDiveAbility::StaticClass()));
	
	Turn_ = CastChecked<UPFTurnAbility>(
		Owner->GetAndActivateComponent(UPFTurnAbility::StaticClass()));
	
	WingVisual_ = CastChecked<UPFWingVisualAbility>(
		Owner->GetAndActivateComponent(UPFWingVisualAbility::StaticClass()));

	WingBeat_ = CastChecked<UPFWingBeatAbility>(
	Owner->GetAndActivateComponent(UPFWingBeatAbility::StaticClass()));

	Roll_ = CastChecked<UPFRollAbility>(
		Owner->GetStateComponent(UPFRollAbility::StaticClass()));
}
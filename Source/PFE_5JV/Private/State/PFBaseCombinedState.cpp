#include "State/PFBaseCombinedState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFBaseCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	GlidePtr_ = CastChecked<UPFGlideAbility>(
		Owner->GetAndActivateComponent(UPFGlideAbility::StaticClass()));
	
	DivePtr_ = CastChecked<UPFDiveAbility>(
		Owner->GetAndActivateComponent(UPFDiveAbility::StaticClass()));
	
	TurnPtr_ = CastChecked<UPFTurnAbility>(
		Owner->GetAndActivateComponent(UPFTurnAbility::StaticClass()));
	
	WingVisualPtr_ = CastChecked<UPFWingVisualAbility>(
		Owner->GetAndActivateComponent(UPFWingVisualAbility::StaticClass()));

	WingBeatPtr_ = CastChecked<UPFWingBeatAbility>(
	Owner->GetAndActivateComponent(UPFWingBeatAbility::StaticClass()));

	RollPtr_ = CastChecked<UPFRollAbility>(
		Owner->GetStateComponent(UPFRollAbility::StaticClass()));
}
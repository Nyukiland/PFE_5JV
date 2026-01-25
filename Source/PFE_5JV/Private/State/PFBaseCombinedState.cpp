#include "State/PFBaseCombinedState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFBaseCombinedState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	GlidePtr_ = CastChecked<UPFGlideAbility>(
		Owner->GetAndActivateComponent(UPFGlideAbility::StaticClass()));
}
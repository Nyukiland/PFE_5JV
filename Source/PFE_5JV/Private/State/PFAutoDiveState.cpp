#include "State/PFAutoDiveState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFAutoDiveState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	GlidePtr_ = CastChecked<UPFGlideAbility>(
		Owner->GetAndActivateComponent(UPFGlideAbility::StaticClass()));
}

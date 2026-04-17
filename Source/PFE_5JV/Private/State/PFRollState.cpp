#include "State/PFRollState.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFRollState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	RollPtr_ = Owner->GetAndActivateComponent<UPFRollAbility>();
}

void UPFRollState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	if (!RollPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[CheckRoll] Bad set up"));
		return;
	}

	if (RollPtr_->Roll(deltaTime))
	{
		Owner->SetDefaultState();
	}
}

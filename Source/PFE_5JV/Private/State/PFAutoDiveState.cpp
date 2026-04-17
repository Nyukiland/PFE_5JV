#include "State/PFAutoDiveState.h"

#include "State/PFBaseCombinedState.h"
#include "State/PFWingBeatSubState.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFAutoDiveState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	DivePtr_ = Owner->GetAndActivateComponent<UPFDiveAbility>();
	WingBeatPtr_ = Owner->GetAndActivateComponent<UPFWingBeatAbility>();

	if (!WingBeatSubState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	AddSubState(WingBeatSubState_);
	
	WingBeatPtr_->OnWingBeatCalled.AddDynamic(
		this,
		&UPFAutoDiveState::OnWingBeat);
}

void UPFAutoDiveState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);

	if (!DivePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	if (DivePtr_)
		DivePtr_->AutoDive(deltaTime);
}

void UPFAutoDiveState::OnWingBeat(int count)
{
	if (!DivePtr_ || !BaseCombinedState_)
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoDiveState] Bad set up"));
		return;	
	}
	
	if (DivePtr_ && DivePtr_->AutoDiveRotationComplete())
		Owner->ChangeState(BaseCombinedState_);
}

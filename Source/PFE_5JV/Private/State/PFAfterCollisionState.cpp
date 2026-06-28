#include "State/PFAfterCollisionState.h"

#include "State/PFBaseCombinedState.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFAfterCollisionState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	CollisionResourcePtr_ = Owner->GetStateComponent<UPFCollisionResource>();
}

void UPFAfterCollisionState::OnTick_Implementation(float deltaTime)
{
	Super::OnTick_Implementation(deltaTime);


	UE_LOG(LogTemp, Error, TEXT("[UPFAfterCollisionState] this should not be used anymore check link"))
		Owner->SetDefaultState();
}

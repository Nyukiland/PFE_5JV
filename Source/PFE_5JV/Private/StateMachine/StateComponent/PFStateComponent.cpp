#include "StateMachine/StateComponent/PFStateComponent.h"

UPFStateComponent::UPFStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPFStateComponent::ComponentEarlyInit_Implementation()
{
	
}

void UPFStateComponent::ComponentInit_Implementation(APFPlayerCharacter* owner)
{
	Owner_ = owner;
}

void UPFStateComponent::ComponentEnable_Implementation()
{
	
}

void UPFStateComponent::ComponentDisable_Implementation()
{
	
}

void UPFStateComponent::ComponentUpdate_Implementation(float deltaTime)
{
	
}


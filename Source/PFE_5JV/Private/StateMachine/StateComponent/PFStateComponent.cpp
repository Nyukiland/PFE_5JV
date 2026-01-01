#include "StateMachine/StateComponent/PFStateComponent.h"

UPFStateComponent::UPFStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPFStateComponent::ComponentEarlyInit_Implementation()
{
	
}

void UPFStateComponent::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Owner = ownerObj;
}

void UPFStateComponent::ComponentEnable_Implementation()
{
	
}

void UPFStateComponent::ComponentDisable_Implementation()
{
	
}

void UPFStateComponent::ComponentTick_Implementation(float deltaTime)
{
	
}


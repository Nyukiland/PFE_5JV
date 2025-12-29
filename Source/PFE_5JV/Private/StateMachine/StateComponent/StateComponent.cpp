#include "StateMachine/StateComponent/StateComponent.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UStateComponent::ComponentEarlyInit_Implementation()
{
	
}

void UStateComponent::ComponentInit_Implementation(APlayerCharacter* owner)
{
	Owner_ = owner;
}

void UStateComponent::ComponentEnable_Implementation()
{
	
}

void UStateComponent::ComponentDisable_Implementation()
{
	
}

void UStateComponent::ComponentUpdate_Implementation(float deltaTime)
{
	
}


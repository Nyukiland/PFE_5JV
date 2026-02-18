#include "StateMachine/StateComponent/PFStateComponent.h"
#include "StateMachine/PFPlayerCharacter.h"

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
	if (!Owner->GetRootComponent())
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] There is no root attached to the player"), *this->GetName())
		return;
	}
	
	PhysicRoot = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
	ForwardRoot = Owner->ForwardRootPtr;
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

FString UPFStateComponent::GetInfo_Implementation()
{
	return TEXT("");
}


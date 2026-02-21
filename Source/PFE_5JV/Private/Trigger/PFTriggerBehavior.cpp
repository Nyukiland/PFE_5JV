#include "Trigger/PFTriggerBehavior.h"

UPFTriggerBehavior::UPFTriggerBehavior()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPFTriggerBehavior::Init(APFTrigger* parentTrigger)
{
	TriggerObjectPtr_ = parentTrigger;
}

void UPFTriggerBehavior::OnTriggerEnter_Implementation()
{
	
}

void UPFTriggerBehavior::OnTriggerExit_Implementation()
{
	
}

void UPFTriggerBehavior::OnTriggerTick_Implementation(float deltaTime)
{
	
}



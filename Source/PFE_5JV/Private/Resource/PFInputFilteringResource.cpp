#include "Resource/PFInputFilteringResource.h"

void UPFInputFilteringResource::ReceiveInputLeft(float value)
{
	InputLeft_ = value;
}

void UPFInputFilteringResource::ReceiveInputRight(float value)
{
	InputRight_ = value;
}

void UPFInputFilteringResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);
}

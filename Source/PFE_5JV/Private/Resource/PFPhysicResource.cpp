#include "Resource/PFPhysicResource.h"

float UPFPhysicResource::GetMaxSpeed() const
{
	return DataPtr_ ? DataPtr_->MaxSpeed : 0;
}

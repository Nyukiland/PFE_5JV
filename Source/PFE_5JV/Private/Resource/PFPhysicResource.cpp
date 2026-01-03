#include "Resource/PFPhysicResource.h"

float UPFPhysicResource::GetMaxSpeed() const
{
	return DataPtr ? DataPtr->MaxSpeed : 0;
}

#include "Resource/PFPhysicResource.h"

#include "StateMachine/PFPlayerCharacter.h"

float UPFPhysicResource::GetMaxSpeed() const
{
	return DataPtr_ ? DataPtr_->MaxSpeed : 0;
}

void UPFPhysicResource::AddForce(FVector force, bool bShouldResetForce, float duration, UCurveFloat* curve)
{
	FForceToAdd forceToAdd(force, bShouldResetForce, duration, curve);
	AllForces_.Add(forceToAdd);
}

void UPFPhysicResource::SetVelocity(FVector velocity)
{
	Velocity_ = velocity;
}

FVector UPFPhysicResource::GetVelocity() const
{
	return Owner->GetVelocity();
}

#include "Resource/PFPhysicResource.h"

#include "StateMachine/PFPlayerCharacter.h"

float UPFPhysicResource::GetMaxSpeed() const
{
	return DataPtr_ ? DataPtr_->MaxSpeed : 0;
}

void UPFPhysicResource::AddForce(float ouais) const
{
	
}

void UPFPhysicResource::AddForce(FVector force, float duration, UCurveFloat* curve) const
{
	// FForceToAdd pouet(force, duration, curve);
	// AllForces_.Add(pouet);
}

void UPFPhysicResource::SetVelocity(FVector velocity)
{
	Velocity_ = velocity;
}

FVector UPFPhysicResource::GetVelocity() const
{
	return Owner->GetVelocity();
}

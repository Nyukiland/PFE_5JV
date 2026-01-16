#include "Resource/PFPhysicResource.h"

#include "StateMachine/PFPlayerCharacter.h"

float UPFPhysicResource::GetCurrentSpeed0to1()
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return 0.0f;
	}

	return FMath::Clamp(GetCurrentVelocity().Length() / DataPtr_->MaxAboveSpeed, 0, 1);
}

float UPFPhysicResource::GetCurrentSpeed0to1InZ()
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return 0.0f;
	}

	if(GetCurrentVelocity().Z > 0)
	{
		return 0.0f;
	}

	return FMath::Clamp(GetCurrentVelocity().Z / DataPtr_->MaxAboveSpeed, 0, 1);
}

void UPFPhysicResource::AddForce(FVector force, bool bShouldResetForce, bool bShouldAddAtTheEnd, float duration,
                                 UCurveFloat* curve)
{
	FForceToAdd forceToAdd(force, bShouldResetForce, bShouldAddAtTheEnd, duration, curve);
	AllForces_.Add(forceToAdd);
}

void UPFPhysicResource::AddForwardForce(float force, bool bShouldResetForce, bool bShouldAddAtTheEnd, float duration,
                                        UCurveFloat* curve)
{
	FForceToAdd forceToAdd(FVector::ForwardVector * force, bShouldResetForce, bShouldAddAtTheEnd, duration, curve);
	ForwardForces_.Add(forceToAdd);
}

FVector UPFPhysicResource::GetCurrentVelocity() const
{
	return PhysicRoot->GetPhysicsLinearVelocity();
}

float UPFPhysicResource::GetAlignmentWithUp() const
{
	FVector forward = PhysicRoot->GetForwardVector();

	return FVector::DotProduct(forward, GetCurrentVelocity());
}

float UPFPhysicResource::GetCurrentAirFriction() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return 0;
	}

	float alignment = GetAlignmentWithUp();

	if (alignment > 0 && DataPtr_->AirFrictionCurveUp)
	{
		return FMath::Lerp(DataPtr_->BaseAirFriction,
		                   DataPtr_->AirFrictionGoingUp,
		                   DataPtr_->AirFrictionCurveUp->GetFloatValue(alignment));
	}
	if (alignment < 0 && DataPtr_->AirFrictionCurveDown)
	{
		alignment = FMath::Abs(alignment);

		return FMath::Lerp(DataPtr_->BaseAirFriction,
		                   DataPtr_->AirFrictionGoingDown,
		                   DataPtr_->AirFrictionCurveDown->GetFloatValue(alignment));
	}

	return DataPtr_->BaseAirFriction;
}

void UPFPhysicResource::ProcessAirFriction(const float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}
	
	float friction = GetCurrentAirFriction();

	FVector dir = Velocity_.GetSafeNormal();
	Velocity_ -= friction * dir;
	
	dir = ForwardVelo_.GetSafeNormal();
	ForwardVelo_ -= friction * dir;

	ForwardVelo_ = ForwardVelo_.GetClampedToSize(0, DataPtr_->MaxAboveSpeed);
	Velocity_ = Velocity_.GetClampedToSize(0, DataPtr_->MaxAboveSpeed);
}

void UPFPhysicResource::ProcessVelocity(const float deltaTime)
{
	FVector velocity = Velocity_;

	for (int i = AllForces_.Num() - 1; i >= 0; i--)
	{
		FForceToAdd* forceToAdd = &AllForces_[i];

		if (!forceToAdd)
		{
			AllForces_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateForce(forceToAdd, deltaTime, Velocity_);

		if (forceToAdd->Duration <= 0)
		{
			AllForces_.RemoveAt(i);
		}

		velocity += toAdd;
	}

	CurrentGlobalVelo_ = velocity;

	FVector velocityForward = ForwardVelo_;

	for (int i = ForwardForces_.Num() - 1; i >= 0; i--)
	{
		FForceToAdd* forceToAdd = &ForwardForces_[i];

		if (!forceToAdd)
		{
			ForwardForces_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateForce(forceToAdd, deltaTime, ForwardVelo_);

		if (forceToAdd->Duration <= 0)
		{
			ForwardForces_.RemoveAt(i);
		}

		velocityForward += toAdd;
	}

	CurrentForwardVelo_ = velocityForward;
	
	velocity += ForwardRoot->GetForwardVector() * velocityForward.Length();

	PhysicRoot->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::ProcessMaxSpeed(const float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}

	FVector velocity = GetCurrentVelocity();
	velocity = velocity.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);

	if (velocity.Length() < DataPtr_->MaxSpeed)
		return;;

	FVector dir = velocity.GetSafeNormal();
	dir *= -1;

	float speedScaled = velocity.Length() - DataPtr_->MaxSpeed;
	float maxSpeedScaled = velocity.Length() - DataPtr_->MaxSpeed;
	float value01 = FMath::Clamp(speedScaled / maxSpeedScaled, 0.f, 1.f);
	
	velocity -= DataPtr_->AboveSpeedFrictionCurve->GetFloatValue(value01) * dir;

	PhysicRoot->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::SetYawRotationForce(float rotation)
{
	AngularVelocity.Z = rotation;
}

void UPFPhysicResource::ProcessAngularVelocity()
{
	PhysicRoot->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
}

void UPFPhysicResource::DoGravity(const float deltaTime)
{
	if (GravityTimer_ < DataPtr_->TimerMaxGravity)
	{
		GravityTimer_ += deltaTime;
		return;
	}
	
	AllForces_.Add(FForceToAdd(DataPtr_->Gravity * FVector::UpVector));
}

void UPFPhysicResource::ResetGravityTimer()
{
	GravityTimer_ = 0;
}

FVector UPFPhysicResource::CalculateForce(FForceToAdd* force, float deltaTime, FVector& VelocityGlobal)
{
	FVector toAdd = force->Force;

	if (force->CurvePtr)
	{
		toAdd = force->Force * force->CurvePtr->GetFloatValue(force->Duration);
	}

	force->Duration -= deltaTime;

	if (!force->bShouldReset)
	{
		VelocityGlobal += toAdd;
		VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);
	}

	if (force->Duration <= 0)
	{
		if (force->bShoudEndAdded)
		{
			VelocityGlobal += toAdd;
			VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);
		}
	}

	return toAdd;
}

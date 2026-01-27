#include "Resource/PFPhysicResource.h"

#include "StateMachine/PFPlayerCharacter.h"

float UPFPhysicResource::GetCurrentSpeedPercentage()
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return 0.0f;
	}

	return FMath::Clamp(GetCurrentVelocity().Length() / DataPtr_->MaxSpeed, 0, 1);
}

float UPFPhysicResource::GetForwardSpeedPercentage(bool bUseMaxAboveSpeed)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return 0.0f;
	}

	float max = bUseMaxAboveSpeed ? DataPtr_->MaxAboveSpeed : DataPtr_->MaxSpeed;
	return FMath::Clamp(CurrentForwardVelo_.Length() / DataPtr_->MaxSpeed, 0, 1);
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

	return FVector::DotProduct(forward, FVector::UpVector);
}

float UPFPhysicResource::GetCurrentAirFriction() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return 0;
	}

	float alignment = GetAlignmentWithUp();

	if (alignment > 0 && DataPtr_->AirFrictionCurveUpPtr)
	{
		return FMath::Lerp(DataPtr_->BaseAirFriction,
							DataPtr_->AirFrictionGoingUp,
							DataPtr_->AirFrictionCurveUpPtr->GetFloatValue(alignment));
	}
	if (alignment < 0 && DataPtr_->AirFrictionCurveDownPtr)
	{
		alignment = FMath::Abs(alignment);

		return FMath::Lerp(DataPtr_->BaseAirFriction,
							DataPtr_->AirFrictionGoingDown,
							DataPtr_->AirFrictionCurveDownPtr->GetFloatValue(alignment));
	}

	return DataPtr_->BaseAirFriction;
}

void UPFPhysicResource::ProcessAirFriction(const float deltaTime)
{
	if (!DataPtr_ || !DataPtr_->FrictionTimerControlCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}

	if (FrictionTimer_ < DataPtr_->TimerMaxFriction)
	{
		FrictionTimer_ += deltaTime;
	}
	
	float friction = GetCurrentAirFriction();
	friction *= FrictionPercentValue();
	friction *= deltaTime;
	
	FVector dir = GlobalVelocity_.GetSafeNormal();
	GlobalVelocity_ -= friction * dir;

	dir = ForwardVelo_.GetSafeNormal();
	ForwardVelo_ -= friction * dir;

	ForwardVelo_ = ForwardVelo_.GetClampedToSize(0, DataPtr_->MaxAboveSpeed);
	GlobalVelocity_ = GlobalVelocity_.GetClampedToSize(0, DataPtr_->MaxAboveSpeed);
}

void UPFPhysicResource::ProcessVelocity(const float deltaTime)
{
	FVector velocity = GlobalVelocity_;

	for (int i = AllForces_.Num() - 1; i >= 0; i--)
	{
		FForceToAdd* forceToAdd = &AllForces_[i];

		if (!forceToAdd)
		{
			AllForces_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateForce(forceToAdd, deltaTime, GlobalVelocity_);

		if (forceToAdd->Timer <= 0)
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

		if (forceToAdd->Timer <= 0)
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
		return;

	FVector dir = velocity.GetSafeNormal();

	float speedScaled = velocity.Length() - DataPtr_->MaxSpeed;
	float maxSpeedScaled = velocity.Length() - DataPtr_->MaxSpeed;
	float value01 = FMath::Clamp(speedScaled / maxSpeedScaled, 0.f, 1.f);

	velocity -= DataPtr_->AboveSpeedFrictionCurvePtr->GetFloatValue(value01) * deltaTime * dir;

	PhysicRoot->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::SetYawRotationForce(float rotation, bool bShouldResetForce, bool bShouldAddAtTheEnd,
											float duration,
											UCurveFloat* curve)
{
	FForceToAdd forceToAdd(FVector(0, 0, rotation), bShouldResetForce, bShouldAddAtTheEnd, duration, curve);
	AngularForces_.Add(forceToAdd);
}

void UPFPhysicResource::ProcessAngularVelocity(const float deltaTime)
{
	FVector velocity = AngularVelocity_;

	for (int i = AngularForces_.Num() - 1; i >= 0; i--)
	{
		FForceToAdd* forceToAdd = &AngularForces_[i];

		if (!forceToAdd)
		{
			AngularForces_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateForce(forceToAdd, deltaTime, AngularVelocity_);

		if (forceToAdd->Timer <= 0)
		{
			AngularForces_.RemoveAt(i);
		}

		velocity += toAdd;
	}

	PhysicRoot->SetPhysicsAngularVelocityInDegrees(velocity);
}

void UPFPhysicResource::SetPitchRotationVisual(float rotation, int priority)
{
	if (priority > PitchPriority_ || FMath::Abs(rotation) < 1)
		return;

	PitchPriority_ = priority;
	PitchRotation_ = rotation;
}

void UPFPhysicResource::ProcessPitchVisual()
{
	FRotator rotation = ForwardRoot->GetRelativeRotation();
	rotation.Pitch = PitchRotation_;
	ForwardRoot->SetRelativeRotation(rotation);

	PitchRotation_ = 0;
	PitchPriority_ = 1000;
}

void UPFPhysicResource::DoGravity(const float deltaTime)
{
	GravityTimer_ += deltaTime;

	if (GravityTimer_ < DataPtr_->TimerMaxGravity)
	{
		return;
	}

	float timeValue = FMath::Clamp((GravityTimer_ - DataPtr_->TimerMaxGravity) / DataPtr_->GravityLerpTime, 0, 1);
	float gravity = FMath::Lerp(0, DataPtr_->Gravity, timeValue);

	AllForces_.Add(FForceToAdd(gravity * FVector::UpVector));
}

void UPFPhysicResource::ResetPhysicsTimer()
{
	GravityTimer_ = 0;
	FrictionTimer_ = 0;
}

float UPFPhysicResource::FrictionPercentValue() const
{
	return DataPtr_->FrictionTimerControlCurvePtr->GetFloatValue(
		FMath::Clamp(FrictionTimer_ / DataPtr_->TimerMaxFriction, 0, 1));
}

FVector UPFPhysicResource::CalculateForce(FForceToAdd* force, float deltaTime, FVector& VelocityGlobal)
{
	FVector toAdd = force->Force;

	if (force->CurvePtr)
	{
		toAdd = force->Force * force->CurvePtr->GetFloatValue((force->Duration - force->Timer)/force->Duration);
	}

	force->Timer -= deltaTime;

	if (!force->bShouldReset)
	{
		VelocityGlobal += toAdd;
		VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);
	}

	if (force->Timer <= 0)
	{
		if (force->bShoudEndAdded)
		{
			VelocityGlobal += toAdd;
			VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(DataPtr_->MaxAboveSpeed);
		}
	}

	return toAdd;
}

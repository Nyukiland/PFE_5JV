#include "Resource/PFPhysicResource.h"

#include "Ability/Data/PFDiveAbilityData.h"
#include "Ability/Data/PFWingBeatAbilityData.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFPhysicResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return;
	}

	ForwardVelocity_ = FVector::ForwardVector * DataPtr_->InitialVelocity;
	CurrentForwardVelocity_ = ForwardVelocity_;
	MaxAbsoluteVelocity_ = GetMaxBoostVelocity();
}

void UPFPhysicResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	ProcessPitchVisual(deltaTime);
	ProcessAngularVelocity(deltaTime);
    
	ProcessAirFriction(deltaTime);
	DoGravity(deltaTime);
	ProcessVelocity(deltaTime);
	ProcessBaseMaxVelocity(deltaTime);
	ProcessOverrideVelocity();
}

FString UPFPhysicResource::GetInfo_Implementation()
{
	FString text = TEXT("<hb>Physic:</>");
	text += TEXT("\n <y.b> Forward Magnitude: ") + FString::Printf(TEXT("%f"), CurrentForwardVelocity_.Length()) +
		TEXT("</>");
	text += TEXT("\n <b>GlobalVelocity: </>") + FString::Printf(TEXT("%s"), *CurrentGlobalVelocity_.ToString());
	text += TEXT("\n");
	text += TEXT("\n <b>Forward Velocity Count: </>") + FString::Printf(TEXT("%i"), ForwardVelocities_.Num());
	text += TEXT("\n <b>Global Velocity Count: </>") + FString::Printf(TEXT("%i"), GlobalVelocities_.Num());
	text += TEXT("\n");
	text += TEXT("\n <b>Friction percent: </>") + FString::Printf(TEXT("%f"), FrictionPercentValue());
	text += TEXT("\n");
	text += TEXT("\n <b>Height: </>") + FString::Printf(TEXT("%f"), Owner->ForwardRootPtr->GetComponentLocation().Z);
	return text;
}

void UPFPhysicResource::SetKinematic(bool bisKinematic)
{
	if (PhysicRoot->IsSimulatingPhysics() == bisKinematic)
		return;
	
	PhysicRoot->SetSimulatePhysics(!bisKinematic);
	if (bisKinematic) PhysicRoot->WakeRigidBody();
}

/* return the max velocity of the most effective method to gain a huge one (Dive or SuperWingBeat)
* (depends on the GD metrics)
*/
float UPFPhysicResource::GetMaxBoostVelocity() const
{
	if (!DataDivePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveResource] No Data available"))
		return 0;
	}
	
	if (!DataWingBeatPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatResource] No Data available"))
		return 0;
	}

	const float maxBoostVelocity = DataDivePtr_->MaxDiveVelocity;
	return maxBoostVelocity;
}

float UPFPhysicResource::GetForwardVelocityPercentage() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] No Data available"))
		return 0.0f;
	}

	return FMath::Clamp(CurrentForwardVelocity_.Length() / DataDivePtr_->MaxDiveVelocity, 0, 1);
}

void UPFPhysicResource::AddVelocity(FVector velocity, bool bShouldResetVelocity, bool bShouldAddAtTheEnd, float duration,
								UCurveFloat* curve)
{
	FVelocityToAdd velocityToAdd(velocity, bShouldResetVelocity, bShouldAddAtTheEnd, duration, curve);
	GlobalVelocities_.Add(velocityToAdd);
}

void UPFPhysicResource::AddForwardVelocity(float velocity, bool bShouldResetVelocity, bool bShouldAddAtTheEnd, float duration,
										UCurveFloat* curve)
{
	FVelocityToAdd velocityToAdd(FVector::ForwardVector * velocity, bShouldResetVelocity, bShouldAddAtTheEnd, duration, curve);
	ForwardVelocities_.Add(velocityToAdd);
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
	if (!DataPtr_ || !DataPtr_->FrictionTimerControlCurvePtr || !DataPtr_->FrictionLostBasedOnSpeedCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}

	if (FrictionTimer_ < DataPtr_->TimerMaxFriction)
	{
		FrictionTimer_ += deltaTime;
	}

	float friction = GetCurrentAirFriction();
	friction *= DataPtr_->FrictionLostBasedOnSpeedCurvePtr->GetFloatValue(GetForwardVelocityPercentage());
	friction *= FrictionPercentValue();
	friction *= deltaTime;

	FVector dir = GlobalVelocity_.Length() > 1 ? GlobalVelocity_.GetSafeNormal() : GlobalVelocity_;
	GlobalVelocity_ -= friction * dir;

	dir = ForwardVelocity_.Length() > 1 ? ForwardVelocity_.GetSafeNormal() : ForwardVelocity_;
	ForwardVelocity_ -= friction * dir;

	ForwardVelocity_ = ForwardVelocity_.GetClampedToSize(0, MaxAbsoluteVelocity_);
	GlobalVelocity_ = GlobalVelocity_.GetClampedToSize(0, MaxAbsoluteVelocity_);
}

void UPFPhysicResource::ProcessVelocity(const float deltaTime)
{
	FVector velocity = GlobalVelocity_;

	for (int i = GlobalVelocities_.Num() - 1; i >= 0; i--)
	{
		FVelocityToAdd* velocityToAdd = &GlobalVelocities_[i];

		if (!velocityToAdd)
		{
			GlobalVelocities_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateVelocity(velocityToAdd, deltaTime, GlobalVelocity_);

		if (velocityToAdd->Timer <= 0)
		{
			GlobalVelocities_.RemoveAt(i);
		}

		velocity += toAdd;
	}

	CurrentGlobalVelocity_ = velocity;

	FVector velocityForward = ForwardVelocity_;

	for (int i = ForwardVelocities_.Num() - 1; i >= 0; i--)
	{
		FVelocityToAdd* velocityToAdd = &ForwardVelocities_[i];

		if (!velocityToAdd)
		{
			ForwardVelocities_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateVelocity(velocityToAdd, deltaTime, ForwardVelocity_);

		if (velocityToAdd->Timer <= 0)
		{
			ForwardVelocities_.RemoveAt(i);
		}

		velocityForward += toAdd;
	}

	CurrentForwardVelocity_ = velocityForward;

	velocity += ForwardRootPtr_->GetForwardVector() * velocityForward.Length();

	PhysicRoot->SetPhysicsLinearVelocity(velocity);
}

// assure that if we are above the base max velocity, we slow down
void UPFPhysicResource::ProcessBaseMaxVelocity(const float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}

	FVector velocity = GetCurrentVelocity();

	// control that we don't exceed system max velocity
	velocity = velocity.GetClampedToMaxSize(MaxAbsoluteVelocity_);

	// control that we don't exceed base velocity
	if (velocity.Length() < DataWingBeatPtr_->MaxVelocityWingBeatVelocity)
		return;

	// If we exceed base velocity, we need to slow down :

	// - take the direction
	FVector dir = velocity.GetSafeNormal();
	// - get how much we exceed base velocity : 
	float velocityScaled = velocity.Length() - DataWingBeatPtr_->MaxVelocityWingBeatVelocity;
	// - get how much boost velocity is above base velocity :
	float maxVelocityScaled = GetMaxBoostVelocity() - DataWingBeatPtr_->MaxVelocityWingBeatVelocity;
	// - get the ratio :
	float ratio = FMath::Clamp(velocityScaled / maxVelocityScaled, 0.f, 1.f);
	// - remove velocity from the friction effect
	velocity -= DataPtr_->AboveBaseMaxVelocityFrictionCurvePtr->GetFloatValue(ratio) * DataPtr_->AboveVelocityFriction * deltaTime * dir;

	PhysicRoot->SetPhysicsLinearVelocity(velocity);
}

void UPFPhysicResource::ProcessOverrideVelocity()
{
	if (FMath::Abs(CurrentOverrideForwardVelocity_) > 0.1f)
	{
		PhysicRoot->SetPhysicsLinearVelocity(ForwardRootPtr_->GetForwardVector() * CurrentOverrideForwardVelocity_);
		CurrentOverrideForwardVelocity_ = 0;
	}
}

void UPFPhysicResource::SetYawRotationVelocity(float rotation, bool bShouldResetVelocity, bool bShouldAddAtTheEnd,
											float duration, UCurveFloat* curve)
{
	const FVelocityToAdd velocityToAdd(FVector(0, 0, rotation), bShouldResetVelocity, bShouldAddAtTheEnd, duration, curve);
	AngularVelocities_.Add(velocityToAdd);
}

void UPFPhysicResource::ProcessAngularVelocity(const float deltaTime)
{
	FVector velocity = AngularVelocity_;

	for (int i = AngularVelocities_.Num() - 1; i >= 0; i--)
	{
		FVelocityToAdd* velocityToAdd = &AngularVelocities_[i];

		if (!velocityToAdd)
		{
			AngularVelocities_.RemoveAt(i);
			continue;
		}

		FVector toAdd = CalculateVelocity(velocityToAdd, deltaTime, AngularVelocity_);

		if (velocityToAdd->Timer <= 0)
		{
			AngularVelocities_.RemoveAt(i);
		}

		velocity += toAdd;
	}

	if (bBlockRight_ && velocity.Z > 0.f)
		velocity.Z = 0.f;
	if (bBlockLeft_ && velocity.Z < 0.f)
		velocity.Z = 0.f;

	if (!AvoidanceNormal_.IsNearlyZero())
	{
		float rightDot = FVector::DotProduct(PhysicRoot->GetRightVector(), AvoidanceNormal_);
        
		if (rightDot < -0.1f && velocity.Z > 0.f)
			velocity.Z = 0.f;
		if (rightDot > 0.1f && velocity.Z < 0.f)
			velocity.Z = 0.f;
	}
	
	PhysicRoot->SetPhysicsAngularVelocityInDegrees(velocity);
}

void UPFPhysicResource::SetAvoidanceNormal(const FVector& Normal)
{
	AvoidanceNormal_ = Normal;
}

void UPFPhysicResource::SetDirectionalBlocks(bool bBlockRight, bool bBlockLeft, bool bBlockUp, bool bBlockDown)
{
	bBlockRight_ = bBlockRight;
	bBlockLeft_ = bBlockLeft;
	bBlockUp_ = bBlockUp;
	bBlockDown_ = bBlockDown;
}

void UPFPhysicResource::AddToPitchRotationVisual(float rotationToAdd, int priority)
{
	if (priority > PitchPriority_ || FMath::Abs(rotationToAdd) < 1)
		return;

	PitchResetRot_ = false;
	PitchPriority_ = priority;
	PitchRotation_ = CurrentPitchValue_ + rotationToAdd;
}

void UPFPhysicResource::SetPitchRotationVisual(float rotation, int priority)
{
	if (priority > PitchPriority_ || FMath::Abs(rotation) < 1)
		return;

	float normalizedRot = FRotator::NormalizeAxis(rotation);
	
	PitchResetRot_ = false;
	PitchPriority_ = priority;
	PitchRotation_ = normalizedRot;
}

void UPFPhysicResource::HardSetPitchRotationVisual(float rotation)
{
	PitchRotation_ = rotation;
	CurrentPitchValue_ = rotation;
	
	FRotator newRot = FRotator::ZeroRotator;
	newRot.Pitch = CurrentPitchValue_;
	ForwardRootPtr_->SetRelativeRotation(newRot);
}

void UPFPhysicResource::ProcessPitchVisual(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[PhysicResource] DataPtr_ is null"));
		return;
	}

	float delta = FMath::FindDeltaAngleDegrees(CurrentPitchValue_, PitchRotation_);
	
	if (bBlockUp_ && delta > 0.f)
		delta = 0.f;
	if (bBlockDown_ && delta < 0.f)
		delta = 0.f;

	if (!AvoidanceNormal_.IsNearlyZero())
	{
		float upDot = FVector::DotProduct(PhysicRoot->GetUpVector(), AvoidanceNormal_);
        
		if (upDot > 0.1f && delta < 0.f)
			delta = 0.f;
		if (upDot < -0.1f && delta > 0.f)
			delta = 0.f;
	}
	
	float speed = 0;
	if (PitchResetRot_)
		speed = (delta > 0.f) ? DataPtr_->PitchRotationLerpVelocityUpGoingToBase : DataPtr_->PitchRotationLerpVelocityDownGoingToBase;
	else
		speed = (delta > 0.f) ? DataPtr_->PitchRotationLerpVelocityUp : DataPtr_->PitchRotationLerpVelocityDown;
	
	CurrentPitchValue_ += delta * FMath::Clamp(speed * deltaTime, 0.f, 1.f);

	FRotator newRot = FRotator::ZeroRotator;
	newRot.Pitch = CurrentPitchValue_;
	ForwardRootPtr_->SetRelativeRotation(newRot);

	bIsFlipped = FMath::FindDeltaAngleDegrees(CurrentPitchValue_, 180) < 90;
	PitchResetRot_ = true;
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

	GlobalVelocities_.Add(FVelocityToAdd(gravity * FVector::UpVector));
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

void UPFPhysicResource::OverrideForwardVelocity(float velocity)
{
	CurrentOverrideForwardVelocity_ = velocity;
}

void UPFPhysicResource::RemoveGlobalVelocities()
{
	GlobalVelocities_.Empty();
}

void UPFPhysicResource::RemoveForwardVelocities()
{
	ForwardVelocities_.Empty();
}

void UPFPhysicResource::RemoveAngularVelocities()
{
	AngularVelocities_.Empty();
}

void UPFPhysicResource::RemoveVelocities()
{
	RemoveGlobalVelocities();
	RemoveForwardVelocities();
}

void UPFPhysicResource::RemoveAllVelocities()
{
	RemoveVelocities();
	RemoveAngularVelocities();
}

void UPFPhysicResource::StopAllMotion()
{
	RemoveAllVelocities();
	ForwardVelocity_ = FVector::ZeroVector;
	GlobalVelocity_ = FVector::ZeroVector;
}

FVector UPFPhysicResource::CalculateVelocity(FVelocityToAdd* velocity, float deltaTime, FVector& VelocityGlobal) const
{
	FVector toAdd = velocity->Velocity;

	if (velocity->CurvePtr)
	{
		toAdd = velocity->Velocity * velocity->CurvePtr->GetFloatValue((velocity->Duration - velocity->Timer) / velocity->Duration);
	}

	velocity->Timer -= deltaTime;

	if (!velocity->bShouldReset)
	{
		VelocityGlobal += toAdd;
		VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(MaxAbsoluteVelocity_);
	}

	if (velocity->Timer <= 0)
	{
		if (velocity->bShoudEndAdded)
		{
			VelocityGlobal += toAdd;
			VelocityGlobal = VelocityGlobal.GetClampedToMaxSize(MaxAbsoluteVelocity_);
		}
	}

	return toAdd;
}

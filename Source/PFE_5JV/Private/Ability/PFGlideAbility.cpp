#include "Ability/PFGlideAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFGlideAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));

	if (DataPtr_)
		Timer_ = DataPtr_->AutoDiveDuration;
}

void UPFGlideAbility::ReceiveInputMovement(FVector2D inputValue)
{
	Input_ = inputValue;
}

void UPFGlideAbility::MovementBasedJoystick()
{
	if (!PhysicResourcePtr_ || !DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[GlideAbility] PhysicResource or Data bad set up"));
		return;
	}

	if (!DataPtr_->bUseJoystickMovement)
		return;

	FVector rightVector = ForwardRoot->GetRightVector();
	PhysicResourcePtr_->AddForce(Input_.X * DataPtr_->RightForce * rightVector);

	float pitchValue = Input_.Y * DataPtr_->PitchForce;
	pitchValue *= DataPtr_->bUseInvertedPitch ? -1.0f : 1.0f;
	PhysicResourcePtr_->SetPitchRotationVisual(pitchValue, 0);
}

void UPFGlideAbility::ConstantMovement(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[GlideAbility] PhysicResource or Data bad set up"));
		return;
	}

	PhysicResourcePtr_->AddForwardForce(DataPtr_->BaseForwardMovement);
}

bool UPFGlideAbility::ShouldGoInAutoDive()
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[GlideAbility] PhysicResource or Data bad set up"));
		return false;
	}

	return PhysicResourcePtr_->CurrentForwardVelo_.Length() < DataPtr_->AutoDiveSpeedLimit;
}

void UPFGlideAbility::StartAutoDive()
{
	Timer_ = 0;
	PhysicResourcePtr_->AddForwardForce(DataPtr_->AutoDiveSpeedGain,
	                                    true,
	                                    true,
	                                    DataPtr_->AutoDiveDuration,
	                                    DataPtr_->AutoDiveSpeedCurve);
}

bool UPFGlideAbility::AutoDiveIfNoSpeed(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_ || !DataPtr_->AutoDiveSpeedCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[GlideAbility] PhysicResource or Data bad set up"));
		return true;
	}

	Timer_ += deltaTime;

	if (Timer_ <= DataPtr_->AutoDiveRotationTime)
	{
		float value0to1 = FMath::Clamp(Timer_ / DataPtr_->AutoDiveRotationTime, 0, 1);
		PhysicResourcePtr_->SetPitchRotationVisual(FMath::Lerp(0, -DataPtr_->AutoDiveDiveRotation, value0to1), -2);
		return Timer_ >= DataPtr_->AutoDiveDuration;
	}
	else if (Timer_ >= (DataPtr_->AutoDiveDuration - DataPtr_->AutoDiveRotationTime))
	{
		float value0to1 = (Timer_ - (DataPtr_->AutoDiveDuration - DataPtr_->AutoDiveRotationTime)) / DataPtr_->
			AutoDiveRotationTime;
		value0to1 = FMath::Clamp(value0to1, 0, 1);
		PhysicResourcePtr_->SetPitchRotationVisual(FMath::Lerp(-DataPtr_->AutoDiveDiveRotation, 0, value0to1), -2);
		return Timer_ >= DataPtr_->AutoDiveDuration;
	}

	PhysicResourcePtr_->SetPitchRotationVisual(-DataPtr_->AutoDiveDiveRotation, -2);

	return Timer_ >= DataPtr_->AutoDiveDuration;
}

#include "Ability/PFDiveAbility.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFDiveAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));

	VisualResourcePtr_ = CastChecked<UPFVisualResource>
		(Owner->GetStateComponent(UPFVisualResource::StaticClass()));
}

void UPFDiveAbility::ComponentDisable_Implementation()
{
	Super::ComponentDisable_Implementation();

	InputLeft_ = 0.0f;
	InputRight_ = 0.0f;
	GetHighestValue();
	TimerAutoDive_ = 0.0f;

	bIsDivingStateGoingUp_ = false;
	SpeedBeforeDive_ = 0.0f;
	GoingUpTimer_ = 10;
	MaxTimeGoingUp_ = 0;
}

void UPFDiveAbility::ReceiveInputLeft(float left)
{
	InputLeft_ = left;
	GetHighestValue();
}

void UPFDiveAbility::ReceiveInputRight(float right)
{
	InputRight_ = right;
	GetHighestValue();
}

void UPFDiveAbility::Dive(float deltaTime)
{
	if (!DataPtr_ || !DataPtr_->DiveAccelerationBasedOnRotationCurvePtr
		|| !DataPtr_->DiveAccelerationBasedOnSpeedCurvePtr || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad Set up on data"));
		return;
	}

	float speedToGive = DataPtr_->ForceToGive *
		DataPtr_->DiveAccelerationBasedOnRotationCurvePtr->GetFloatValue(HighestInput_);

	float velocity0to1 = PhysicResourcePtr_->GetForwardSpeedPercentage(true);
	speedToGive *= DataPtr_->DiveAccelerationBasedOnSpeedCurvePtr->GetFloatValue(velocity0to1);

	PhysicResourcePtr_->AddForwardForce(speedToGive * deltaTime, false);
}

void UPFDiveAbility::AfterDiveGoingUp(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_
		|| !DataPtr_->GoingUpRotationCurve
		|| !DataPtr_->GoingUpDurationCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad Set up on data"));
		return;
	}

	if (!DataPtr_->bUseUCurveGoingUp)
		return;

	if (IsDiving() && !bIsDivingStateGoingUp_)
	{
		bIsDivingStateGoingUp_ = true;
		GoingUpTimer_ = 100;
		SpeedBeforeDive_ = PhysicResourcePtr_->GetCurrentVelocity().Length();
	}

	if (!IsDiving() && bIsDivingStateGoingUp_)
	{
		bIsDivingStateGoingUp_ = false;

		float value01 = PhysicResourcePtr_->GetCurrentVelocity().Length() - SpeedBeforeDive_;
		value01 /= PhysicResourcePtr_->GetMaxSpeed();
		value01 = FMath::Clamp(value01, 0.0f, 1.0f);
		MaxTimeGoingUp_ = DataPtr_->MaxTimeForGoingUpAfterDive;
		MaxTimeGoingUp_ *= DataPtr_->GoingUpDurationCurve->GetFloatValue(value01);
		GoingUpTimer_ = 0;
	}

	if (bIsDivingStateGoingUp_ || GoingUpTimer_ > MaxTimeGoingUp_)
		return;

	GoingUpTimer_ += deltaTime;

	float timer01 = FMath::Clamp(GoingUpTimer_ / MaxTimeGoingUp_, 0, 1);
	float rotationValue = DataPtr_->MaxUpRotationPitch;
	rotationValue *= DataPtr_->GoingUpRotationCurve->GetFloatValue(timer01);
	PhysicResourcePtr_->SetPitchRotationVisual(rotationValue, -1);
}

void UPFDiveAbility::DiveVisual(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return;
	}

	float highestValue = FMath::Min(InputLeft_, InputRight_);

	float lerpSpeedToUse = highestValue >= CurrentMedianValue_
		                       ? DataPtr_->LerpPitchSpeedGoingUp
		                       : DataPtr_->LerpPitchSpeedGoingDown;

	CurrentMedianValue_ = FMath::Lerp(CurrentMedianValue_, highestValue, lerpSpeedToUse * deltaTime);

	float value = FMath::Lerp(0, DataPtr_->MaxRotationPitch, CurrentMedianValue_);
	PhysicResourcePtr_->SetPitchRotationVisual(value, -1);
}

void UPFDiveAbility::DiveRoll(float deltaTime)
{
	if (!DataPtr_ || !VisualResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return;
	}

	DiveRollInputRecording(deltaTime);
	DiveRollCheck();

	if (DiveRollDirection == 0)
	{
		return;
	}

	VisualResourcePtr_->AddToRollRotation(DataPtr_->DiveRollRotationForce * DiveRollDirection, -3);
}

bool UPFDiveAbility::IsDiving() const
{
	return HighestInput_ != 0;
}

void UPFDiveAbility::AutoDive(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return;
	}

	TimerAutoDive_ += deltaTime;

	float value = TimerAutoDive_ / DataPtr_->AutoDiveRotationTime;
	value = FMath::Clamp(value, 0.0f, 1.0f);
	value = FMath::Lerp(0, DataPtr_->AutoDiveDiveRotationPercentage, value);

	ReceiveInputLeft(value);
	ReceiveInputRight(value);
}

bool UPFDiveAbility::AutoDiveComplete() const
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return true;
	}

	return TimerAutoDive_ > DataPtr_->AutoDiveRotationTime;
}

void UPFDiveAbility::GetHighestValue()
{
	HighestInput_ = FMath::Min(InputLeft_, InputRight_);
}

void UPFDiveAbility::DiveRollInputRecording(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return;
	}

	if (RecordedPreviousInputLeft_ > InputLeft_)
		RecordedPreviousInputLeft_ = InputLeft_;

	if (RecordedPreviousInputRight_ > InputRight_)
		RecordedPreviousInputRight_ = InputRight_;

	if (DiveRollDirection != 0)
		return;
	
	TimerInputRecording_ += deltaTime;

	if (TimerInputRecording_ > DataPtr_->TimeBeforeInputRegister)
	{
		TimerInputRecording_ = 0;

		RecordedPreviousInputLeft_ = InputLeft_;
		RecordedPreviousInputRight_ = InputRight_;
	}
}

void UPFDiveAbility::DiveRollCheck()
{
	if (!IsDiving())
	{
		DiveRollDirection = 0;
		return;
	}
	
	if (FMath::Abs(RecordedPreviousInputLeft_ - InputLeft_) > DataPtr_->DiveRollInputChangeNeeded
		&& InputRight_ == 1)
	{
		DiveRollDirection = -1;
	}
	else if (FMath::Abs(RecordedPreviousInputRight_ - InputRight_) > DataPtr_->DiveRollInputChangeNeeded
		&& InputLeft_ == 1)
	{
		DiveRollDirection = 1;
	}
	else
	{
		DiveRollDirection = 0;
	}
}

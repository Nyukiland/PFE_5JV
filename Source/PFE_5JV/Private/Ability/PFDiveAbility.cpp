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
	Timer_ = 0.0f;
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

void UPFDiveAbility::DiveVisual(float deltaTime)
{
	if (!DataPtr_ || !PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
		return;
	}
	
	float highestValue = FMath::Min(InputLeft_, InputRight_);
	
	float lerpSpeedToUse = highestValue >= CurrentMedianValue_ ?
	DataPtr_->LerpPitchSpeedGoingUp : DataPtr_->LerpPitchSpeedGoingDown;
		
	CurrentMedianValue_ = FMath::Lerp(CurrentMedianValue_, highestValue, lerpSpeedToUse * deltaTime);

	float value = FMath::Lerp(0, DataPtr_->MaxRotationPitch, CurrentMedianValue_);
	PhysicResourcePtr_->SetPitchRotationVisual(value, -1);
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

	Timer_ += deltaTime;

	float value = Timer_/DataPtr_->AutoDiveRotationTime;
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

	return Timer_ > DataPtr_->AutoDiveRotationTime;
}

void UPFDiveAbility::GetHighestValue()
{
	HighestInput_ = FMath::Min(InputLeft_, InputRight_);
}

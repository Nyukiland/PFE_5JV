#include "Ability/PFWingVisualAbility.h"

void UPFWingVisualAbility::ReceiveInputRight(float right)
{
	InputRight_ = right;
}

void UPFWingVisualAbility::ReceiveInputLeft(float left)
{
	InputLeft_ = left;
}

void UPFWingVisualAbility::ChangeRotation(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingVisual] Bad set up on Data"))
		return;
	}
	
	float highestValue = FMath::Min(InputLeft_, InputRight_);
	
	float lerpSpeedToUse = highestValue >= CurrentMedianValue_ ?
	DataPtr_->LerpPitchSpeedGoingUp : DataPtr_->LerpPitchSpeedGoingDown;
		
	CurrentMedianValue_ = FMath::Lerp(CurrentMedianValue_, highestValue, lerpSpeedToUse * deltaTime);

	FRotator rotation = ForwardRoot->GetRelativeRotation();
	rotation.Pitch = FMath::Lerp(0, DataPtr_->MaxRotationPitch, CurrentMedianValue_);
	ForwardRoot->SetRelativeRotation(rotation);
}

void UPFWingVisualAbility::ChangeWingRotation(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingVisual] Bad set up on Data"))
		return;
	}
	
	FRotator rightRotation = RightWingRoot_->GetRelativeRotation();
	FRotator leftRotation = LeftWingRoot_->GetRelativeRotation();

	float rotValue = FMath::Max(InputLeft_, InputRight_) - FMath::Min(InputLeft_, InputRight_);
	rotValue *= InputLeft_ > InputRight_ ? 1 : -1;

	rightRotation.Roll = FMath::Lerp(rightRotation.Roll, rotValue * DataPtr_->MaxWingRotation,
		DataPtr_->LerpWingRotation);

	leftRotation.Roll = FMath::Lerp(leftRotation.Roll, -rotValue * DataPtr_->MaxWingRotation,
		DataPtr_->LerpWingRotation);

	RightWingRoot_->SetRelativeRotation(rightRotation);
	LeftWingRoot_->SetRelativeRotation(leftRotation);
}

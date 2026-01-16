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
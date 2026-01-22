#include "Ability/PFWingVisualAbility.h"

#include "StateMachine/PFPlayerCharacter.h"

void UPFWingVisualAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
}

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

	float value = FMath::Lerp(0, DataPtr_->MaxRotationPitch, CurrentMedianValue_);
	PhysicResourcePtr_->SetPitchRotationVisual(value, -1);
}

void UPFWingVisualAbility::WingTurnVisu(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingVisual] Bad set up on Data"))
		return;
	}

	float rotValue = FMath::Max(InputLeft_, InputRight_) - FMath::Min(InputLeft_, InputRight_);
	rotValue *= InputRight_ > InputLeft_ ? 1 : -1;

	FRotator birdRot = BirdVisualPtr_->GetRelativeRotation();
	birdRot.Pitch = FMath::Lerp(birdRot.Pitch, DataPtr_->MaxRotationPitch * rotValue, DataPtr_->LerpWingRotation * deltaTime);
	BirdVisualPtr_->SetRelativeRotation(birdRot);
}

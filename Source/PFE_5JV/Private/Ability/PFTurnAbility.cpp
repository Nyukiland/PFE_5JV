#include "Ability/PFTurnAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFTurnAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));

	VisualResourcePtr_ = CastChecked<UPFVisualResource>
		(Owner->GetStateComponent(UPFVisualResource::StaticClass()));
}

void UPFTurnAbility::ReceiveInputLeft(float left)
{
	InputLeft_ = left;
	GetRotationValue();
}

void UPFTurnAbility::ReceiveInputRight(float right)
{
	InputRight_ = right;
	GetRotationValue();
}

void UPFTurnAbility::Turn(float deltaTime)
{
	if (!DataPtr_ || !DataPtr_->RotationForceBasedOnInput
		|| !DataPtr_->RotationForceBasedOnVelocity)
	{
		UE_LOG(LogTemp, Error, TEXT("[TurnAbility] Bad set up on data"));
		return;
	}

	if (RotationValue_ == 0)
	{
		PhysicResourcePtr_->SetYawRotationForce(0);
		return;
	}
	
	float value = DataPtr_->RotationForce;
	value *= FMath::Sign(RotationValue_);
	value *= DataPtr_->RotationForceBasedOnInput->GetFloatValue(FMath::Abs(RotationValue_));
	float velocity0to1 = PhysicResourcePtr_->GetForwardSpeedPercentage();
	value *= DataPtr_->RotationForceBasedOnVelocity->GetFloatValue(velocity0to1);
	
	PhysicResourcePtr_->SetYawRotationForce(value);
}

void UPFTurnAbility::TurnVisual(float deltaTime)
{
	if (!DataPtr_ || !VisualResourcePtr_ || !VisualResourcePtr_->BirdVisualPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingVisual] BirdVisualPtr_ or Bad set up on Data"))
		return;
	}

	FRotator birdRot = VisualResourcePtr_->BirdVisualPtr_->GetRelativeRotation();
	birdRot.Pitch = FMath::Lerp(birdRot.Pitch, DataPtr_->MaxWingRotation * -RotationValue_, DataPtr_->LerpWingRotation * deltaTime);
	VisualResourcePtr_->BirdVisualPtr_->SetRelativeRotation(birdRot);
}

void UPFTurnAbility::GetRotationValue()
{
	float rotValue = FMath::Max(InputLeft_, InputRight_) - FMath::Min(InputLeft_, InputRight_);
	rotValue *= InputLeft_ > InputRight_ ? -1 : 1;
	
	if (DataPtr_ && FMath::Abs(rotValue) > DataPtr_->ToleranceRotBetweenInput)
	{
		RotationValue_ = rotValue;
		return;
	}

	RotationValue_ = 0;
}

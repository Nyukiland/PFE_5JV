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

void UPFTurnAbility::ComponentDisable_Implementation()
{
	Super::ComponentDisable_Implementation();

	InputLeft_ = 0.0f;
	InputRight_ = 0.0f;
	GetRotationValue();
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
	if (!DataPtr_ || !DataPtr_->RotationForceBasedOnInputPtr
		|| !DataPtr_->RotationForceBasedOnVelocityPtr)
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
	value *= DataPtr_->RotationForceBasedOnInputPtr->GetFloatValue(FMath::Abs(RotationValue_));
	float velocity0to1 = PhysicResourcePtr_->GetForwardSpeedPercentage();
	value *= DataPtr_->RotationForceBasedOnVelocityPtr->GetFloatValue(velocity0to1);
	
	PhysicResourcePtr_->SetYawRotationForce(value);
}

void UPFTurnAbility::TurnVisual()
{
	if (!DataPtr_ || !VisualResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingVisual] BirdVisualPtr_ or Bad set up on Data"))
		return;
	}

	VisualResourcePtr_->SetRollRotation(DataPtr_->MaxWingRotation * -RotationValue_, 0);
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

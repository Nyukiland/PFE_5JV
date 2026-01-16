#include "Ability/PFTurnAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFTurnAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
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
	if (!DataPtr_ || !DataPtr_->RotationForceBasedOnInput)
	{
		UE_LOG(LogTemp, Error, TEXT("[TurnAbility] Bad set up on data"));
		return;
	}

	if (RotationValue_ == 0)
	{
		PhysicResource_->SetYawRotationForce(0);
		return;
	}
	
	float value = DataPtr_->RotationForce;
	value *= FMath::Sign(RotationValue_);
	value *= DataPtr_->RotationForceBasedOnInput->GetFloatValue(FMath::Abs(RotationValue_));

	PhysicResource_->SetYawRotationForce(value);
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

#include "Ability/PFGlideAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFGlideAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
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

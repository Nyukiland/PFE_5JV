#include "Ability/PFRollAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFRollAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
}

void UPFRollAbility::CallRoll(int sideRoll)
{
	Timer_ = 0;
	RotationDir_ = sideRoll;
	bIsRollComplete_ = false;

	PrevRotator_ = BirdVisualPtr_->GetRelativeRotation();
	PrevQuat_ = PrevRotator_.Quaternion();
}

bool UPFRollAbility::Roll(float deltaTime)
{
	if (!Data_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Roll] Bad Set up on Data"));
		return false;
	}

	if (Timer_ >= Data_->RollDuration)
	{
		if (!bIsRollComplete_)
		{
			BirdVisualPtr_->SetRelativeRotation(PrevRotator_);
			bIsRollComplete_ = true;
		}

		return true;
	}

	Timer_ += deltaTime;

	float value = FMath::Clamp(Timer_ / Data_->RollDuration, 0, 1);

	FVector rightVector = ForwardRoot->GetRightVector();
	PhysicResourcePtr_->AddForce(Data_->RollForce * Data_->RollForceOverTime->GetFloatValue(value) * rightVector * RotationDir_);

	float totalDegrees = Data_->RotationCount * 360.f * -RotationDir_;
	FQuat rollQuat = FQuat(FVector::ForwardVector,
							FMath::DegreesToRadians(totalDegrees * value));

	const FQuat NewQuat = rollQuat * PrevQuat_;

	BirdVisualPtr_->SetRelativeRotation(NewQuat);

	return false;
}

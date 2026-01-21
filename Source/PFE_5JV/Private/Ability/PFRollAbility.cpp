#include "Ability/PFRollAbility.h"

#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFRollAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
}

void UPFRollAbility::CallRoll(int sideRoll)
{
	Timer_ = 0;
	RotationDir_ = sideRoll;
	bIsRollComplete_ = false;
	AccumulatedRollDegrees_ = 0;
	PrevRotator_ = BirdVisual->GetRelativeRotation();
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
			BirdVisual->SetRelativeRotation(PrevRotator_);
			bIsRollComplete_ = true;
		}
		
		return true;
	}

	Timer_ += deltaTime;

	float value = FMath::Clamp(Timer_/Data_->RollDuration, 0, 1);
	
	FVector rightVector = ForwardRoot->GetRightVector();
	PhysicResource_->AddForce(Data_->RollForce * Data_->RollForceOverTime->GetFloatValue(value) * rightVector);
	
	float totalDegrees = Data_->RotationCount * 360.f;
	float targetDegrees = totalDegrees * value;
	float deltaDegrees = targetDegrees - AccumulatedRollDegrees_;

	AccumulatedRollDegrees_ = targetDegrees;

	FRotator BirdRot = BirdVisual->GetRelativeRotation();
	BirdRot.Pitch += deltaDegrees * RotationDir_;
	BirdVisual->SetRelativeRotation(BirdRot);
	
	return false;
}

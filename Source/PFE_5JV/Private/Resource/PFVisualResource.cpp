#include "Resource/PFVisualResource.h"

FRotator UPFVisualResource::GetBirdVisualRotation() const
{
	return BirdVisualPtr_->GetRelativeRotation();
}

void UPFVisualResource::SetBirdVisualRotation(FRotator rotation, int priority)
{
	if (priority > BirdVisuPriority_ || rotation.IsNearlyZero())
		return;

	BirdVisuPriority_ = priority;
	BirdVisuRotation_ = rotation;
}

void UPFVisualResource::SetRollRotation(float rotation, int priority)
{
	if (priority > RollPriority_ || FMath::Abs(rotation) < 1)
		return;

	RollPriority_ = priority;
	RollRotation_ = rotation;
}

void UPFVisualResource::ProcessRollRotation(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[VisualResource] No Data available"));
		return;
	}
	
	FRotator birdRot = BirdVisualPtr_->GetRelativeRotation();
	birdRot.Pitch = FMath::Lerp(birdRot.Pitch, RollRotation_, DataPtr_->LerpWingRotation * deltaTime);
	BirdVisualPtr_->SetRelativeRotation(birdRot);

	RollRotation_ = 0;
	RollPriority_ = 1000;
}

void UPFVisualResource::ProcessBirdVisuRotation(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[VisualResource] No Data available"));
		return;
	}
	
	FQuat CurrentQuat = BirdVisualPtr_->GetRelativeRotation().Quaternion();
	const FQuat TargetQuat  = BirdVisuRotation_.Quaternion();
	FQuat NewQuat = FQuat::Slerp(CurrentQuat, TargetQuat, DataPtr_->LerpWingRotation * deltaTime);
	
	BirdVisualPtr_->SetRelativeRotation(NewQuat);
	
	BirdVisuRotation_ = BaseBirdVisuRotator_;
	BirdVisuPriority_ = 1000;
}

void UPFVisualResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	BaseBirdVisuRotator_ = BirdVisualPtr_->GetRelativeRotation();
}

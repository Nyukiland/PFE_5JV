#include "Resource/PFVisualResource.h"

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

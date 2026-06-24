#include "Resource/PFVisualResource.h"

void UPFVisualResource::AddToRollRotation(float rotationToAdd, int priority)
{
	if (priority > RollPriority_ || FMath::Abs(rotationToAdd) < 1.f)
		return;

	RollPriority_ = priority;
	bIsContinuousRoll_ = true;
	ContinuousRollStep_ = rotationToAdd; 
}

void UPFVisualResource::SetRollRotation(float rotation, int priority)
{
	if (priority > RollPriority_ || FMath::Abs(rotation) < 1.f)
		return;

	RollPriority_ = priority;
	bIsContinuousRoll_ = false;
	RollRotation_ = rotation;
}

void UPFVisualResource::ProcessRollRotation(float deltaTime)
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[VisualResource] No Data available"));
		return;
	}
    
	if (bIsContinuousRoll_)
	{
		// Bypass the shortest-path Lerp for continuous infinite spins.
		// Directly add the requested step.
		CurrentRollValue_ += ContinuousRollStep_;
	}
	else
	{
		// Use the shortest-path Lerp for absolute targets (like banking).
		float delta = FMath::FindDeltaAngleDegrees(CurrentRollValue_, RollRotation_);
		float speed = DataPtr_->LerpWingRotation;
		CurrentRollValue_ += delta * FMath::Clamp(speed * deltaTime, 0.f, 1.f);
	}

	CurrentRollValue_ = FRotator::NormalizeAxis(CurrentRollValue_);

	FRotator birdRot = BirdVisualRootPtr_->GetRelativeRotation();
	birdRot.Roll = CurrentRollValue_;
	BirdVisualRootPtr_->SetRelativeRotation(birdRot);

	// Reset pipeline for the next tick
	RollRotation_ = 0.f;
	bIsContinuousRoll_ = false;
	ContinuousRollStep_ = 0.f;
	RollPriority_ = 1000;
}

void UPFVisualResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	BaseBirdVisuRotator_ = BirdVisualRootPtr_->GetRelativeRotation();
}

void UPFVisualResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	ProcessRollRotation(deltaTime);
}

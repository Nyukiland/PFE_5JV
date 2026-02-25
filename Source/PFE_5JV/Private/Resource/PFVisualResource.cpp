#include "Resource/PFVisualResource.h"

FRotator UPFVisualResource::GetBirdVisualRotation() const
{
	return BirdVisualPtr_->GetRelativeRotation();
}

void UPFVisualResource::AddToRollRotation(float rotationToAdd, int priority)
{
	if (priority > RollPriority_ || FMath::Abs(rotationToAdd) < 1)
		return;

	RollPriority_ = priority;
	RollRotation_ = CurrentRollValue_ + rotationToAdd;
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
	
	float delta = FMath::FindDeltaAngleDegrees(CurrentRollValue_, RollRotation_);

	float speed = DataPtr_->LerpWingRotation;
	CurrentRollValue_ += delta * FMath::Clamp(speed * deltaTime, 0.f, 1.f);

	CurrentRollValue_ = FRotator::NormalizeAxis(CurrentRollValue_);

	FRotator birdRot = BirdVisualPtr_->GetRelativeRotation();
	birdRot.Roll = CurrentRollValue_;
	BirdVisualPtr_->SetRelativeRotation(birdRot);

	RollRotation_ = 0.f;
	RollPriority_ = 1000;

}

void UPFVisualResource::ChangeMeshMaterial(int const MaterialIndex, EStateMaterial NewStateMaterial) const
{
	switch(NewStateMaterial)
	{
		case EStateMaterial::ESM_Normal :
			BirdMeshObjectPtr_->SetMaterial(MaterialIndex, DataPtr_->NormalMaterialPtr_);
			break;
		case EStateMaterial::ESM_SuperWingBeatPossible :
			BirdMeshObjectPtr_->SetMaterial(MaterialIndex, DataPtr_->SuperBeatWingMaterialPossiblePtr_);
			break;
		default:
			BirdMeshObjectPtr_->SetMaterial(MaterialIndex, DataPtr_->NormalMaterialPtr_);
	}
	
}

EStateMaterial UPFVisualResource::GetMeshMaterial(int const MaterialIndex) const
{
	UMaterialInterface* MaterialPtr = BirdMeshObjectPtr_->GetMaterial(MaterialIndex);
	if(MaterialPtr == DataPtr_->NormalMaterialPtr_)
	{
		return EStateMaterial::ESM_Normal;
	}
	
	if(MaterialPtr == DataPtr_->SuperBeatWingMaterialPossiblePtr_)
	{
		return EStateMaterial::ESM_SuperWingBeatPossible;
	}
	
	return EStateMaterial::ESM_Normal;
}

void UPFVisualResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	BaseBirdVisuRotator_ = BirdVisualPtr_->GetRelativeRotation();
}

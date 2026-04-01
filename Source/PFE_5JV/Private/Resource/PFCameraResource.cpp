#include "Resource/PFCameraResource.h"
#include <algorithm>
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "Helpers/PFMathHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCameraResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicReferencePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
	DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();

	CameraPositionPtr_->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraPositionPtr_->SetWorldLocation(PhysicRoot->GetComponentLocation());
	CameraPositionPtr_->SetWorldRotation(FRotator::ZeroRotator);
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!CheckValidity())
		return;

	ManageCameraPitch(deltaTime);
	ManageCameraYaw(deltaTime);

	ManageCameraOffset(deltaTime);
	ManageCameraDistance(deltaTime);
}

bool UPFCameraResource::CheckValidity() const
{
	if (!CameraPtr_ || !CameraPositionPtr_ || !CameraYawPtr_ || !CameraPitchPtr_ || !CameraRollPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] One or more Gimbal Components are NULL"));
		return false;
	}
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("The Data referenced in PlayerCharacter blueprint is NULL"))
		return false;
	}

	if (!PhysicReferencePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("The PhysicResource referenced in PlayerCharacter blueprint is NULL"))
		return false;
	}

	if (!DiveAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("The VisualResource referenced in PlayerCharacter blueprint is NULL"))
		return false;
	}

	if (!TurnAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("The TurnAbility referenced in PlayerCharacter blueprint is NULL"))
		return false;
	}
	return true;
}

void UPFCameraResource::ManageCameraOffset(float deltaTime)
{
	FVector pos = ForwardRootPtr_->GetComponentLocation();

	// Turn
	float desiredTurnValue = DataPtr_->TurnYOffset * TurnAbilityPtr_->TurnValue();
	TurnCurrentOffset_ = FMath::Lerp(TurnCurrentOffset_, desiredTurnValue,
									DataPtr_->OffsetTurnLerpSpeed * deltaTime);

	// Height (Dive, Base, WingBeat)
	float heightLerpToUse = DiveAbilityPtr_->IsDiving()
								? DataPtr_->OffsetGoToDiveLerpSpeed
								: DataPtr_->OffsetGoToBaseLerpSpeed;
	float desiredHeightValue = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->DiveZOffset,
											DiveAbilityPtr_->GetDivingValue());
	HeightCurrentOffset_ = FMath::Lerp(HeightCurrentOffset_, desiredHeightValue,
										heightLerpToUse * deltaTime);

	// Apply in absolute because main component
	pos += TurnCurrentOffset_ * ForwardRootPtr_->GetRightVector();
	pos += HeightCurrentOffset_ * ForwardRootPtr_->GetUpVector();
	CameraPositionPtr_->SetWorldLocation(pos);
}

void UPFCameraResource::ManageCameraDistance(float deltaTime)
{
	float desiredDistanceValue = FMath::Lerp(DataPtr_->BaseDistanceWithPlayer, DataPtr_->MaxDistanceWithPlayer,
											PhysicReferencePtr_->GetForwardVelocityPercentage());
	DistanceCurrentOffset_ = FMath::Lerp(DistanceCurrentOffset_, desiredDistanceValue,
										DataPtr_->DistanceLerpSpeed * deltaTime);

	//Forward -> X
	CameraPtr_->SetRelativeLocation(FVector(DistanceCurrentOffset_, 0, 0));
}

void UPFCameraResource::ManageCameraPitch(float deltaTime)
{
	CameraPitchPtr_->SetRelativeRotation(ForwardRootPtr_->GetRelativeRotation());
}

void UPFCameraResource::ManageCameraYaw(float deltaTime)
{
	CameraYawPtr_->SetRelativeRotation(PhysicRoot->GetRelativeRotation());
}

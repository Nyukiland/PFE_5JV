#include "Resource/PFCameraResource.h"
#include <algorithm>
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "StateMachine/PFPlayerCharacter.h"

UPFCameraResource::UPFCameraResource()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UPFCameraResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
	DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();
	WingBeatAbilityPtr_ = Owner->GetStateComponent<UPFWingBeatAbility>();

	CameraPositionPtr_->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraPositionPtr_->SetWorldLocation(PhysicRoot->GetComponentLocation());
	CameraPositionPtr_->SetWorldRotation(FRotator::ZeroRotator);

	if (!CheckValidity())
	{
		return;
	}

	CurrentPitch_ = ForwardRootPtr_->GetRelativeRotation().Pitch;
	CurrentYaw_ = PhysicRoot->GetRelativeRotation().Yaw;
	DistanceCurrentOffset_ = DataPtr_->BaseDistanceWithPlayer;
	HeightCurrentOffset_ = DataPtr_->BaseZOffset;
	TurnCurrentOffset_ = 0.f;
	TrueCameraPitch_ = DataPtr_->BaseCamRotation;

	CameraPitchPtr_->SetRelativeRotation(FRotator(CurrentPitch_, 0.f, 0.f));
	CameraYawPtr_->SetRelativeRotation(FRotator(0.f, CurrentYaw_, 0.f));
	CameraPtr_->SetRelativeLocation(FVector(-DistanceCurrentOffset_, 0.f, 0.f));
	CameraPtr_->SetRelativeRotation(FRotator(TrueCameraPitch_, 0.f, 0.f));
}

// Regular tick to make sure physic movement is applied before doing the tick
void UPFCameraResource::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CheckValidity())
		return;

	ManageCameraPitch(DeltaTime);
	ManageCameraYaw(DeltaTime);

	ManageCameraOffset(DeltaTime);
	ManageCameraDistance(DeltaTime);
	ManageTrueCameraPitch(DeltaTime);
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
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The Data referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!DataPtr_->DistanceCurve || !DataPtr_->DiveOffsetCurve
		|| !DataPtr_->TurnOffsetCurve || !DataPtr_->WingBeatOffsetCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The Data Curve in CameraResource blueprint is NULL"))
		return false;
	}
	
	if (!PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The PhysicResource referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!DiveAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The VisualResource referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!TurnAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The TurnAbility referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!WingBeatAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The WingBeatAbility referenced in CameraResource blueprint is NULL"))
		return false;
	}
	
	return true;
}

void UPFCameraResource::ManageCameraOffset(float deltaTime)
{
	FVector pos = ForwardRootPtr_->GetComponentLocation();

	// Turn
	float turnValue = TurnAbilityPtr_->TurnValue();
	float desiredTurnValue = DataPtr_->TurnOffsetCurve->GetFloatValue(FMath::Abs(turnValue)) * FMath::Sign(turnValue);
	desiredTurnValue *= DataPtr_->TurnOffsetSpeedCurve->GetFloatValue(PhysicResourcePtr_->GetForwardVelocityPercentage());
	desiredTurnValue *= DataPtr_->TurnYOffset;
	
	float turnLerpToUse = TurnAbilityPtr_->TurnValue() == 0?
		DataPtr_->OffsetTurnToBaseLerpSpeed : DataPtr_->OffsetTurnLerpSpeed;
	
	TurnCurrentOffset_ = FMath::FInterpTo(TurnCurrentOffset_, desiredTurnValue,
									deltaTime, turnLerpToUse);

	// Height (Dive, Base, WingBeat)
	float targetHeight = DataPtr_->BaseZOffset;
	float heightLerpToUse = DataPtr_->OffsetGoToBaseLerpSpeed;

	float wingBeatValue = WingBeatAbilityPtr_->GetCurrentWingBeatPercentage();
	if (wingBeatValue > 0)
	{
		float percentage = DataPtr_->WingBeatOffsetCurve->GetFloatValue(wingBeatValue);
		targetHeight = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->WingBeatZOffset, percentage);
		heightLerpToUse = DataPtr_->OffsetGoToWingBeatLerpSpeed;
	}

	float diveValue = DiveAbilityPtr_->GetDivingValue();
	if (diveValue > 0)
	{
		float percentage = DataPtr_->WingBeatOffsetCurve->GetFloatValue(diveValue);
		targetHeight = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->DiveZOffset, percentage);
		heightLerpToUse = DataPtr_->OffsetGoToDiveLerpSpeed;
	}
	
	HeightCurrentOffset_ = FMath::FInterpTo(HeightCurrentOffset_, targetHeight,
										deltaTime, heightLerpToUse);

	// Apply in absolute because main component
	pos += TurnCurrentOffset_ * ForwardRootPtr_->GetRightVector();
	pos += HeightCurrentOffset_ * ForwardRootPtr_->GetUpVector();
	CameraPositionPtr_->SetWorldLocation(pos);
}

void UPFCameraResource::ManageCameraDistance(float deltaTime)
{
	float desiredDistanceValue = FMath::Lerp(DataPtr_->BaseDistanceWithPlayer, DataPtr_->MaxDistanceWithPlayer,
											PhysicResourcePtr_->GetForwardVelocityPercentage());
	DistanceCurrentOffset_ = FMath::FInterpTo(DistanceCurrentOffset_, desiredDistanceValue,
										deltaTime, DataPtr_->DistanceLerpSpeed);

	//Forward -> X
	CameraPtr_->SetRelativeLocation(FVector(-DistanceCurrentOffset_, 0, 0));
}

void UPFCameraResource::ManageCameraPitch(float deltaTime)
{
	float targetPitch = ForwardRootPtr_->GetRelativeRotation().Pitch;
	
	float delta = FMath::FindDeltaAngleDegrees(CurrentPitch_, targetPitch);
	delta = FMath::Clamp(delta, -DataPtr_->MaxPitchAngle, DataPtr_->MaxPitchAngle);

	CurrentPitch_ = targetPitch - delta;
	CurrentPitch_ = FMath::FInterpTo(CurrentPitch_, CurrentPitch_ + delta,
		deltaTime, DataPtr_->PitchLagSpeed);
	CurrentPitch_ = FRotator::NormalizeAxis(CurrentPitch_);

	CameraPitchPtr_->SetRelativeRotation(FRotator(CurrentPitch_, 0.f, 0.f));
}

void UPFCameraResource::ManageCameraYaw(float deltaTime)
{
	float targetYaw = PhysicRoot->GetRelativeRotation().Yaw;

	float delta = FMath::FindDeltaAngleDegrees(CurrentYaw_, targetYaw);
	delta = FMath::Clamp(delta, -DataPtr_->MaxYawAngle, DataPtr_->MaxYawAngle);

	CurrentYaw_ = targetYaw - delta;
	CurrentYaw_ = FMath::FInterpTo(CurrentYaw_, CurrentYaw_ + delta,
		deltaTime, DataPtr_->YawLagSpeed);
	CurrentYaw_ = FRotator::NormalizeAxis(CurrentYaw_);

	CameraYawPtr_->SetRelativeRotation(FRotator(0.f, CurrentYaw_, 0.f));
}

void UPFCameraResource::ManageTrueCameraPitch(float deltaTime)
{
	float targetPitch = DataPtr_->BaseCamRotation;
	float lerpToUse = DataPtr_->CamRotLerpSpeedToBase;

	float wingBeatValue = WingBeatAbilityPtr_->GetCurrentWingBeatPercentage();
	if (wingBeatValue > 0)
	{
		float percentage = DataPtr_->WingBeatCamRotCurve->GetFloatValue(wingBeatValue);
		targetPitch = FMath::Lerp(DataPtr_->BaseCamRotation, DataPtr_->WingBeatCamRotation, percentage);
		lerpToUse = DataPtr_->CamRotLerpSpeedToWingBeat;
	}

	float diveValue = DiveAbilityPtr_->GetDivingValue();
	if (diveValue > 0)
	{
		float percentage = DataPtr_->DiveCamRotCurve->GetFloatValue(diveValue);
		targetPitch = FMath::Lerp(DataPtr_->BaseCamRotation, DataPtr_->DiveCamRotation, percentage);
		lerpToUse = DataPtr_->CamRotLerpSpeedToDive;
	}

	TrueCameraPitch_ = FMath::FInterpTo(TrueCameraPitch_, targetPitch,
		deltaTime, lerpToUse);
	CameraPtr_->SetRelativeRotation(FRotator(TrueCameraPitch_, 0.f, 0.f));
}

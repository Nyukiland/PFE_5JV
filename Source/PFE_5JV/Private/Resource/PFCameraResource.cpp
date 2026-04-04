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

	PhysicReferencePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
	TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
	DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();

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

	CameraPitchPtr_->SetRelativeRotation(FRotator(CurrentPitch_, 0.f, 0.f));
	CameraYawPtr_->SetRelativeRotation(FRotator(0.f, CurrentYaw_, 0.f));
	CameraPtr_->SetRelativeLocation(FVector(-DistanceCurrentOffset_, 0.f, 0.f));
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
	TurnCurrentOffset_ = FMath::FInterpTo(TurnCurrentOffset_, desiredTurnValue,
									deltaTime, DataPtr_->OffsetTurnLerpSpeed);

	// Height (Dive, Base, WingBeat)
	float heightLerpToUse = DiveAbilityPtr_->IsDiving()
								? DataPtr_->OffsetGoToDiveLerpSpeed
								: DataPtr_->OffsetGoToBaseLerpSpeed;
	float desiredHeightValue = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->DiveZOffset,
											DiveAbilityPtr_->GetDivingValue());
	HeightCurrentOffset_ = FMath::FInterpTo(HeightCurrentOffset_, desiredHeightValue,
										deltaTime, heightLerpToUse);

	// Apply in absolute because main component
	pos += TurnCurrentOffset_ * ForwardRootPtr_->GetRightVector();
	pos += HeightCurrentOffset_ * ForwardRootPtr_->GetUpVector();
	CameraPositionPtr_->SetWorldLocation(pos);
}

void UPFCameraResource::ManageCameraDistance(float deltaTime)
{
	float desiredDistanceValue = FMath::Lerp(DataPtr_->BaseDistanceWithPlayer, DataPtr_->MaxDistanceWithPlayer,
											PhysicReferencePtr_->GetForwardVelocityPercentage());
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

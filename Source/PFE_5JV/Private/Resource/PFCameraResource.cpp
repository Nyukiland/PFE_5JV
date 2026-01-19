#include "Resource/PFCameraResource.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "StateMachine/PFPlayerCharacter.h"

UPFCameraResource::UPFCameraResource()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPFCameraResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	DiveAbility_ = CastChecked<UPFDiveAbility>(Owner->GetStateComponent(UPFDiveAbility::StaticClass()));

	if (!CameraRootPtr_)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the CameraRoot is null"));
	}
	if (!SpringArmPtr_)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the SpringArm is null"));
	}
	if (!CameraPtr_)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the Camera is null"));
	}

	CameraRootPtr_->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
}

void UPFCameraResource::UpdateRotation(float deltaTime)
{
	if (!CameraResourceData_)
		return;
	
	FRotator OwnerRot_ = Owner->GetActorRotation();
	FRotator CameraRot_ = CameraRootPtr_->GetComponentRotation();
	FRotator DeltaRot_ = (OwnerRot_ - CameraRot_).GetNormalized();
	
	const bool IsDiving_ = IsDiveActive_ ||
							DiveTransitionTimer_ < CameraResourceData_->DivingEndInterpDuration;
	if (!IsDiving_)
	{
		// Limit Yaw and Pitch max span
		DeltaRot_.Yaw   = FMath::Clamp(DeltaRot_.Yaw,   -CameraResourceData_->MaxYawWhenRotating, CameraResourceData_->MaxYawWhenRotating);
		DeltaRot_.Pitch = FMath::Clamp(DeltaRot_.Pitch, -CameraResourceData_->MaxPitchWhenRotating, CameraResourceData_->MaxPitchWhenRotating);
	}
	DeltaRot_.Roll  = 0.f;

	FRotator TargetRotation_ = CameraRot_ + DeltaRot_;
	FRotator CurrentRotation_ = CameraRootPtr_->GetComponentRotation();

	FRotator NewRotation_ = FMath::RInterpTo(
		CurrentRotation_,
		TargetRotation_,
		deltaTime,
		CameraResourceData_->RotationInterpSpeed
	);

	CameraRootPtr_->SetWorldRotation(NewRotation_);
}

FVector UPFCameraResource::ComputeTargetLocation() const
{
	if (!CameraResourceData_)
		return Owner->GetActorLocation();
	
	FVector Back_ = -Owner->GetActorForwardVector() * CameraResourceData_->BackDistance;
	FVector Up_ = FVector(0.f, 0.f, CameraResourceData_->UpDistance);

	return Owner->GetActorLocation() + Back_ + Up_;
}

void UPFCameraResource::UpdatePosition(float deltaTime)
{
	if (!CameraResourceData_)
		return;
	
	FVector CurrentLocation_ = CameraRootPtr_->GetComponentLocation();
	FVector TargetLocation_ = ComputeTargetLocation();

	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation_,
		TargetLocation_,
		deltaTime,
		CameraResourceData_->PositionInterpSpeed
	);

	CameraRootPtr_->SetWorldLocation(NewLocation);
}

void UPFCameraResource::UpdateZoom(float deltaTime)
{
	if (!CameraResourceData_)
		return;

	if (DiveAbility_->IsDiving()) DiveTheTimer_ += deltaTime;
	else DiveTheTimer_ = 0.f;
	
	// Detect transition
	IsDiveActive_ = DiveTheTimer_ > 1.f;
	if (IsDiveActive_ != WasDiving_)
	{
		DiveTransitionTimer_ = 0.f;
		WasDiving_ = IsDiveActive_;

		ZoomStartDistance_ = SpringArmPtr_->TargetArmLength;
		ZoomStartRotation_ = SpringArmPtr_->GetRelativeRotation();
	}

	DiveTransitionTimer_ += deltaTime;
	
	float DivingInterpDuration_ = IsDiveActive_
	? CameraResourceData_->DivingStartInterpDuration
	: CameraResourceData_->DivingEndInterpDuration;

	float Alpha_ = DiveTransitionTimer_ / DivingInterpDuration_;
	Alpha_ = FMath::Clamp(Alpha_, 0.f, 1.f);
	
	// Distance
	float TargetDistance_ = IsDiveActive_
		? CameraResourceData_->DivingDistance
		: CameraResourceData_->BackDistance;

	SpringArmPtr_->TargetArmLength = FMath::Lerp(
		ZoomStartDistance_,
		TargetDistance_,
		Alpha_
	);

	// Look downward
	FRotator TargetRotation_ = IsDiveActive_
		? CameraResourceData_->DivingRotation
		: FRotator::ZeroRotator;

	SpringArmPtr_->SetRelativeRotation(FMath::Lerp(
		ZoomStartRotation_,
		TargetRotation_,
		Alpha_));
}
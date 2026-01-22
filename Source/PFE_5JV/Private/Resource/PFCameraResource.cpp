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

	DiveAbilityPtr_ = CastChecked<UPFDiveAbility>(Owner->GetStateComponent(UPFDiveAbility::StaticClass()));

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
	
	FRotator ownerRot = Owner->GetActorRotation();
	FRotator cameraRot = CameraRootPtr_->GetComponentRotation();
	FRotator deltaRot = (ownerRot - cameraRot).GetNormalized();
	
	const bool isDiving = IsDiveActive_ ||
							DiveTransitionTimer_ < CameraResourceData_->DivingEndInterpDuration;
	if (!isDiving)
	{
		// Limit Yaw and Pitch max span
		deltaRot.Yaw   = FMath::Clamp(deltaRot.Yaw,   -CameraResourceData_->MaxYawWhenRotating, CameraResourceData_->MaxYawWhenRotating);
		deltaRot.Pitch = FMath::Clamp(deltaRot.Pitch, -CameraResourceData_->MaxPitchWhenRotating, CameraResourceData_->MaxPitchWhenRotating);
	}
	deltaRot.Roll  = 0.f;

	FRotator targetRotation = cameraRot + deltaRot;
	FRotator currentRotation = CameraRootPtr_->GetComponentRotation();

	FRotator newRotation = FMath::RInterpTo(
		currentRotation,
		targetRotation,
		deltaTime,
		CameraResourceData_->RotationInterpSpeed
	);

	CameraRootPtr_->SetWorldRotation(newRotation);
}

FVector UPFCameraResource::ComputeTargetLocation() const
{
	if (!CameraResourceData_)
		return Owner->GetActorLocation();
	
	FVector back = -Owner->GetActorForwardVector() * CameraResourceData_->BackDistance;
	FVector up = FVector(0.f, 0.f, CameraResourceData_->UpDistance);

	return Owner->GetActorLocation() + back + up;
}

void UPFCameraResource::UpdatePosition(float deltaTime)
{
	if (!CameraResourceData_)
		return;
	
	FVector currentLocation = CameraRootPtr_->GetComponentLocation();
	FVector targetLocation = ComputeTargetLocation();

	FVector newLocation = FMath::VInterpTo(
		currentLocation,
		targetLocation,
		deltaTime,
		CameraResourceData_->PositionInterpSpeed
	);

	CameraRootPtr_->SetWorldLocation(newLocation);
}

void UPFCameraResource::UpdateZoom(float deltaTime)
{
	if (!CameraResourceData_)
		return;

	if (DiveAbilityPtr_->IsDiving()) DiveTheTimer_ += deltaTime;
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
	
	float divingInterpDuration = IsDiveActive_
	? CameraResourceData_->DivingStartInterpDuration
	: CameraResourceData_->DivingEndInterpDuration;

	float alpha = DiveTransitionTimer_ / divingInterpDuration;
	alpha = FMath::Clamp(alpha, 0.f, 1.f);
	
	// Distance
	float targetDistance = IsDiveActive_
		? CameraResourceData_->DivingDistance
		: CameraResourceData_->BackDistance;

	SpringArmPtr_->TargetArmLength = FMath::Lerp(
		ZoomStartDistance_,
		targetDistance,
		alpha
	);

	// Look downward
	FRotator targetRotation = IsDiveActive_
		? CameraResourceData_->DivingRotation
		: FRotator::ZeroRotator;

	SpringArmPtr_->SetRelativeRotation(FMath::Lerp(
		ZoomStartRotation_,
		targetRotation,
		alpha));
}
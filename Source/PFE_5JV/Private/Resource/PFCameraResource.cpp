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
	
	FRotator OwnerRot = Owner->GetActorRotation();
	FRotator CameraRot = CameraRootPtr_->GetComponentRotation();

	FRotator DeltaRot = (OwnerRot - CameraRot).GetNormalized();

	// Limit Yaw and Pitch max span
	DeltaRot.Yaw   = FMath::Clamp(DeltaRot.Yaw,   -CameraResourceData_->MaxYawWhenRotating, CameraResourceData_->MaxYawWhenRotating);
	DeltaRot.Pitch = FMath::Clamp(DeltaRot.Pitch, -CameraResourceData_->MaxPitchWhenRotating, CameraResourceData_->MaxPitchWhenRotating);
	DeltaRot.Roll  = 0.f;

	FRotator TargetRotation = CameraRot + DeltaRot;

	// Decomment next line if the roll makes you sick to imit camera movements
	TargetRotation.Roll *= CameraResourceData_->RollFactor;
	/*TargetRotation.Roll = FMath::Clamp(
	TargetRotation.Roll * RollFactor,
	-25.f,
	25.f
	);*/

	FRotator CurrentRotation = CameraRootPtr_->GetComponentRotation();

	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		deltaTime,
		CameraResourceData_->RotationInterpSpeed
	);

	CameraRootPtr_->SetWorldRotation(NewRotation);
}

FVector UPFCameraResource::ComputeTargetLocation() const
{
	if (!CameraResourceData_)
		return Owner->GetActorLocation();
	
	FVector Back = -Owner->GetActorForwardVector() * CameraResourceData_->Distance;
	FVector Up = FVector(0.f, 0.f, CameraResourceData_->Height);
	FVector Forward = Owner->GetActorForwardVector() * CameraResourceData_->LookAhead;

	return Owner->GetActorLocation() + Back + Up + Forward;
}

void UPFCameraResource::UpdatePosition(float deltaTime)
{
	if (!CameraResourceData_)
		return;
	
	FVector CurrentLocation = CameraRootPtr_->GetComponentLocation();
	FVector TargetLocation = ComputeTargetLocation();

	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		deltaTime,
		CameraResourceData_->PositionInterpSpeed
	);

	CameraRootPtr_->SetWorldLocation(NewLocation);
}

void UPFCameraResource::UpdateZoom(float deltaTime)
{
	if (!CameraResourceData_)
		return;
	
	if (DiveAbility_ && DiveAbility_->IsDiving())
	{
		DiveTheTimer += deltaTime;
	}
	else
	{
		DiveTheTimer = 0.f;
	}

	float TargetDistance = CameraResourceData_->Distance;

	if (DiveTheTimer > 1.f)
	{
		TargetDistance = 480.f;
	}

	SpringArmPtr_->TargetArmLength = FMath::FInterpTo(
		SpringArmPtr_->TargetArmLength,
		TargetDistance,
		deltaTime,
		2.f
	);
}
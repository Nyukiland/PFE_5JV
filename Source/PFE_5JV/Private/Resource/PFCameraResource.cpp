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

	if (!CameraRoot)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the CameraRoot is null"));
	}
	if (!SpringArm)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the SpringArm is null"));
	}
	if (!Camera)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CameraResource] The reference to the Camera is null"));
	}

	CameraRoot->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
}

void UPFCameraResource::UpdateRotation(float DeltaTime)
{
	FRotator TargetRotation = Owner->GetActorRotation();

	// Décommenter la ligne d'apres si le roll rends malade pour limiter les mouvements de camera
	TargetRotation.Roll *= RollFactor;
	/*TargetRotation.Roll = FMath::Clamp(
	TargetRotation.Roll * RollFactor,
	-25.f,
	25.f
	);*/

	FRotator CurrentRotation = CameraRoot->GetComponentRotation();

	FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		TargetRotation,
		DeltaTime,
		RotationInterpSpeed
	);

	CameraRoot->SetWorldRotation(NewRotation);
}

FVector UPFCameraResource::ComputeTargetLocation() const
{
	FVector Back = -Owner->GetActorForwardVector() * Distance;
	FVector Up = FVector(0.f, 0.f, Height);
	FVector Forward = Owner->GetActorForwardVector() * LookAhead;

	return Owner->GetActorLocation() + Back + Up + Forward;
}

void UPFCameraResource::UpdatePosition(float DeltaTime)
{
	FVector CurrentLocation = CameraRoot->GetComponentLocation();
	FVector TargetLocation = ComputeTargetLocation();

	FVector NewLocation = FMath::VInterpTo(
		CurrentLocation,
		TargetLocation,
		DeltaTime,
		PositionInterpSpeed
	);

	CameraRoot->SetWorldLocation(NewLocation);
} 
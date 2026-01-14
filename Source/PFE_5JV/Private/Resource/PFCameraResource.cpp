#include "Resource/PFCameraResource.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "StateMachine/PFPlayerCharacter.h"

UPFCameraResource::UPFCameraResource()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPFCameraResource::BeginPlay()
{
	Super::BeginPlay();

	ensureMsgf(CameraRoot, TEXT("[CameraResource] CameraRoot is null"));
	ensureMsgf(SpringArm, TEXT("[CameraResource] SpringArm is null"));
	ensureMsgf(Camera, TEXT("[CameraResource] Camera is null"));
}

void UPFCameraResource::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CameraRoot || !Camera || !GetOwner())
	{
		return;
	}

	UpdateRotation(DeltaTime);
	UpdatePosition(DeltaTime);
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
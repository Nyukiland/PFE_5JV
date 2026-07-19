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
	ProximityResourcePtr_ = Owner->GetStateComponent<UPFProximityResource>();

	CameraPositionPtr_->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	CameraPositionPtr_->SetWorldLocation(PhysicRoot->GetComponentLocation());
	CameraPositionPtr_->SetWorldRotation(FRotator::ZeroRotator);

	OwnerWorldPtr_ = Owner->GetWorld();

	if (!CheckValidity())
	{
		return;
	}

	CurrentPitch_ = ForwardRootPtr->GetRelativeRotation().Pitch;
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
		|| !DataPtr_->TurnOffsetCurve || !DataPtr_->WingBeatOffsetCurve || !DataPtr_->GroundProximitySpeedCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] The Data Curve in CameraResource blueprint is NULL"))
		return false;
	}

	if (!PhysicResourcePtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The PhysicResource referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!DiveAbilityPtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The VisualResource referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!TurnAbilityPtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The TurnAbility referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!WingBeatAbilityPtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The WingBeatAbility referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!ProximityResourcePtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The ProximityResourcePtr_ referenced in CameraResource blueprint is NULL"))
		return false;
	}

	if (!OwnerWorldPtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[UPFCameraResource] The OwnerWorld referenced in CameraResource blueprint is NULL"))
		return false;
	}

	return true;
}

void UPFCameraResource::ManageCameraOffset(float deltaTime)
{
	FVector pos = ForwardRootPtr->GetComponentLocation();

	// Turn
	float turnValue = TurnAbilityPtr_->TurnValue();
	float desiredTurnValue = DataPtr_->TurnOffsetCurve->GetFloatValue(FMath::Abs(turnValue)) * FMath::Sign(turnValue);
	desiredTurnValue *= DataPtr_->TurnOffsetSpeedCurve->GetFloatValue(
		PhysicResourcePtr_->GetForwardVelocityPercentage());
	desiredTurnValue *= DataPtr_->TurnYOffset;

	float turnLerpToUse = TurnAbilityPtr_->TurnValue() == 0
							? DataPtr_->OffsetTurnToBaseLerpSpeed
							: DataPtr_->OffsetTurnLerpSpeed;

	TurnCurrentOffset_ = FMath::FInterpTo(TurnCurrentOffset_, desiredTurnValue,
										deltaTime, turnLerpToUse);

	// Height (Dive, Base, WingBeat)
	float targetHeight = DataPtr_->BaseZOffset;
	float heightLerpToUse = DataPtr_->OffsetGoToBaseLerpSpeed;

	float valuePitch = PhysicResourcePtr_->CurrentPitchValue_;

	if (valuePitch > 0)
	{
		float percentage = valuePitch / WingBeatAbilityPtr_->GetMaxWingBeatAngle();
		percentage = FMath::Clamp(percentage, 0.0f, 1.0f);
		percentage = DataPtr_->WingBeatOffsetCurve->GetFloatValue(percentage);
		targetHeight = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->WingBeatZOffset, percentage);
		heightLerpToUse = DataPtr_->OffsetGoToWingBeatLerpSpeed;
	}

	if (valuePitch <= 0)
	{
		float percentage = valuePitch / DiveAbilityPtr_->GetMaxDivingAngle();
		percentage = FMath::Clamp(percentage, 0.0f, 1.0f);
		percentage = DataPtr_->DiveOffsetCurve->GetFloatValue(percentage);
		targetHeight = FMath::Lerp(DataPtr_->BaseZOffset, DataPtr_->DiveZOffset, percentage);
		heightLerpToUse = DataPtr_->OffsetGoToDiveLerpSpeed;
	}

	// Ground height override
	if (ProximityResourcePtr_->HitBelowResult.bBlockingHit)
	{
		float currentDist = ProximityResourcePtr_->HitBelowResult.Distance;
		float maxDist = ProximityResourcePtr_->GetMaxDistanceWithBelow();

		float groundAlpha = 1.0f - FMath::Clamp(currentDist / maxDist, 0.0f, 1.0f);

		float groundTargetHeight = DataPtr_->BaseZOffset + DataPtr_->GroundZOffset;

		float speedPercent = DataPtr_->GroundProximitySpeedCurve->GetFloatValue(
			PhysicResourcePtr_->GetForwardVelocityPercentage());
		targetHeight = FMath::Lerp(targetHeight, groundTargetHeight, groundAlpha * speedPercent);

		heightLerpToUse = FMath::Max(heightLerpToUse, DataPtr_->GroundAvoidanceLerpSpeed);
	}

	HeightCurrentOffset_ = FMath::FInterpTo(HeightCurrentOffset_, targetHeight, deltaTime, heightLerpToUse);

	// Apply in absolute because main component
	pos += TurnCurrentOffset_ * ForwardRootPtr->GetRightVector();
	pos += HeightCurrentOffset_ * ForwardRootPtr->GetUpVector();
	CameraPositionPtr_->SetWorldLocation(pos);
}

void UPFCameraResource::ManageCameraDistance(float deltaTime)
{
	float desiredDistanceValue = FMath::Lerp(DataPtr_->BaseDistanceWithPlayer, DataPtr_->MaxDistanceWithPlayer,
											PhysicResourcePtr_->GetForwardVelocityPercentage());

	float interpSpeed = (desiredDistanceValue < DistanceCurrentOffset_)
							? DataPtr_->CollisionPushSpeed
							: DataPtr_->DistanceLerpSpeed;

	DistanceCurrentOffset_ = FMath::FInterpTo(DistanceCurrentOffset_, desiredDistanceValue, deltaTime, interpSpeed);

	float closeProximityAlpha = 1.0f - FMath::Clamp((DistanceCurrentOffset_ - DataPtr_->MinCameraDistance) /
													(DataPtr_->BaseDistanceWithPlayer - DataPtr_->MinCameraDistance),
													0.0f, 1.0f);

	float dynamicZOffset = FMath::Lerp(0.0f, DataPtr_->SquishedHeightOffset, closeProximityAlpha);

	FVector idealLocalPos = FVector(-DistanceCurrentOffset_, 0, dynamicZOffset);

	FTransform parentTransform = CameraPositionPtr_->GetComponentTransform();
	FVector startPos = Owner->GetActorLocation();
	FVector endPos = parentTransform.TransformPosition(idealLocalPos);
    
	FVector targetCollisionPos = parentTransform.InverseTransformPosition(CheckRayCameraValidity(startPos, endPos));

	FVector currentCameraPos = CameraPtr_->GetRelativeLocation();
    
	float finalLerpSpeed = (targetCollisionPos.SizeSquared() < currentCameraPos.SizeSquared()) 
						   ? DataPtr_->CollisionPushSpeed : DataPtr_->DistanceLerpSpeed;

	FVector smoothedPos = FMath::VInterpTo(currentCameraPos, targetCollisionPos, deltaTime, finalLerpSpeed);

	CameraPtr_->SetRelativeLocation(smoothedPos);
}

FVector UPFCameraResource::CheckRayCameraValidity(FVector startPos, FVector endPos)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);
	queryParams.bTraceComplex = true;
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->SphereDetectionSize);
    
	FVector currentStart = startPos;
	FVector currentEnd = endPos;
	FHitResult hit;

	const int32 MaxSlideIterations = 3;

	for (int32 i = 0; i < MaxSlideIterations; ++i)
	{
		if (!OwnerWorldPtr_->SweepSingleByChannel(hit, currentStart, currentEnd, FQuat::Identity, ECC_Visibility, sweepShape, queryParams))
		{
			return currentEnd;
		}

		FVector safeHitLoc = hit.Location;
        
		if (hit.bStartPenetrating)
		{
			if (!OwnerWorldPtr_->LineTraceSingleByChannel(hit, currentStart, currentEnd, ECC_Visibility, queryParams))
			{
				return currentEnd;
			}
			safeHitLoc += hit.ImpactNormal * 2.0f; 
		}
		else
		{
			safeHitLoc += hit.ImpactNormal * 2.0f;
		}

		float totalDist = (currentEnd - currentStart).Size();
		float distTraveled = (hit.Location - currentStart).Size();
		float remainingDist = FMath::Max(0.0f, totalDist - distTraveled);

		FVector dir = (currentEnd - currentStart).GetSafeNormal();
		FVector slideDir = FVector::VectorPlaneProject(dir, hit.ImpactNormal);
		slideDir.Normalize();

		currentStart = safeHitLoc;
		currentEnd = safeHitLoc + (slideDir * remainingDist);
	}

	return currentEnd;
}

void UPFCameraResource::ManageCameraPitch(float deltaTime)
{
	float targetPitch = ForwardRootPtr->GetRelativeRotation().Pitch;

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

	float valuePitch = PhysicResourcePtr_->CurrentPitchValue_;

	if (valuePitch > 0)
	{
		float percentage = valuePitch / WingBeatAbilityPtr_->GetMaxWingBeatAngle();
		percentage = FMath::Clamp(percentage, 0.0f, 1.0f);
		percentage = DataPtr_->WingBeatCamRotCurve->GetFloatValue(percentage);
		targetPitch = FMath::Lerp(DataPtr_->BaseCamRotation, DataPtr_->WingBeatCamRotation, percentage);
		lerpToUse = DataPtr_->CamRotLerpSpeedToWingBeat;
	}

	if (valuePitch <= 0)
	{
		float percentage = valuePitch / DiveAbilityPtr_->GetMaxDivingAngle();
		percentage = FMath::Clamp(percentage, 0.0f, 1.0f);
		percentage = DataPtr_->DiveCamRotCurve->GetFloatValue(percentage);
		targetPitch = FMath::Lerp(DataPtr_->BaseCamRotation, DataPtr_->DiveCamRotation, percentage);
		lerpToUse = DataPtr_->CamRotLerpSpeedToDive;
	}

	if (ProximityResourcePtr_->HitBelowResult.bBlockingHit)
	{
		float currentDist = ProximityResourcePtr_->HitBelowResult.Distance;
		float maxDist = ProximityResourcePtr_->GetMaxDistanceWithBelow();

		float groundAlpha = 1.0f - FMath::Clamp(currentDist / maxDist, 0.0f, 1.0f);

		float speedPercent = DataPtr_->GroundProximitySpeedCurve->GetFloatValue(
			PhysicResourcePtr_->GetForwardVelocityPercentage());
		targetPitch = FMath::Lerp(targetPitch, DataPtr_->GroundPitch, groundAlpha * speedPercent);
		lerpToUse = FMath::Max(lerpToUse, DataPtr_->GroundAvoidanceLerpSpeed);
	}

	TrueCameraPitch_ = FMath::FInterpTo(TrueCameraPitch_, targetPitch,
										deltaTime, lerpToUse);
	CameraPtr_->SetRelativeRotation(FRotator(TrueCameraPitch_, 0.f, 0.f));
}

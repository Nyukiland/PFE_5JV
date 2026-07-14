#include "Resource/CollisionPreset/PFAssistedAvoidancePreset.h"
#include "Resource/PFCollisionResource.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFAssistedAvoidancePreset::InitPreset_Implementation(UPFCollisionResource* collisionResource)
{
	Super::InitPreset_Implementation(collisionResource);

	PhysicResourcePtr_ = OwnerPtr_->GetStateComponent<UPFPhysicResource>();
	OwnerWorldPtr_ = OwnerPtr_->GetWorld();
}

void UPFAssistedAvoidancePreset::OnTickActions_Implementation(float deltaTime)
{
	Super::OnTickActions_Implementation(deltaTime);

	if (!DataPtr_ || !PhysicResourcePtr_ || !CollisionResourcePtr_ || !OwnerWorldPtr_)
	{
		UE_LOG(LogTemp, Error,
				TEXT("[AssistedAvoidancePreset] Error In set up, either couldn't get a var or data is null"))
		return;
	}

	ProcessAssistedAvoidance(deltaTime, PhysicResourcePtr_->GetCurrentVelocity());
}

void UPFAssistedAvoidancePreset::ProcessAssistedAvoidance(float deltaTime, const FVector& currentVelocity)
{
	FVector startPos = CollisionResourcePtr_->PhysicRoot->GetComponentLocation();
	FVector forwardDir = CollisionResourcePtr_->ForwardRootPtr->GetForwardVector();

	if (CheckAbsoluteShield(deltaTime, currentVelocity, startPos, forwardDir)) 
		return;

	float distanceThisFrame = currentVelocity.Length() * deltaTime;
	float speedPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
	speedPercentage = DataPtr_->AssistSpeedMultiplierCurve->GetFloatValue(speedPercentage);
	float speedMultiplier = FMath::Lerp(DataPtr_->MinSpeedMultiplier, DataPtr_->MaxSpeedMultiplier, speedPercentage);

	float rayDist[9] = {
		distanceThisFrame + (DataPtr_->AssistDistance * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier),
		distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier)
	};

	FVector totalRepulsion, firstOpenDir;
	float closestDistance;
	bool bHitCenter;
	int openRayCount;

	CalculateDynamicRays(startPos, forwardDir, rayDist, totalRepulsion, firstOpenDir, closestDistance, bHitCenter,
						openRayCount);

	FVector intentDir;
	CalculateClampedIntent(forwardDir, intentDir);

	JudgeAndApplyPredictiveForces(deltaTime, forwardDir, intentDir, totalRepulsion, firstOpenDir, closestDistance,
								distanceThisFrame + (DataPtr_->AvoidDistance * speedMultiplier), speedMultiplier,
								bHitCenter, openRayCount);
}

bool UPFAssistedAvoidancePreset::CheckAbsoluteShield(float deltaTime, const FVector& currentVelocity,
													const FVector& startPos, const FVector& forwardDir)
{
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);
	FHitResult criticalHit;
	bool bHit = OwnerWorldPtr_->SweepSingleByChannel(criticalHit, startPos,
		startPos + (currentVelocity * deltaTime) + (forwardDir * DataPtr_->CriticalBrakeDistance),
		FQuat::Identity, ECC_Visibility, sweepShape, queryParams);
	
	if (bHit && criticalHit.Distance < DataPtr_->CriticalBrakeDistance)
	{
		FVector velocityProjected = FVector::VectorPlaneProject(currentVelocity, criticalHit.ImpactNormal);
		CollisionResourcePtr_->PhysicRoot->SetPhysicsLinearVelocity(velocityProjected);
		PhysicResourcePtr_->CurrentForwardVelocity_ = PhysicResourcePtr_->CurrentForwardVelocity_.GetSafeNormal() * velocityProjected.Length();
		CurrentRepulsion_ = criticalHit.ImpactNormal;
		return true;
	}
	return false;
}

void UPFAssistedAvoidancePreset::CalculateClampedIntent(const FVector& forwardDir, FVector& outIntentDir)
{
	FVector rawIntent = FRotator(PhysicResourcePtr_->GetCurrentPitchRotation(),
								CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw + PhysicResourcePtr_->GetYawAngularVelocity(), 0.f).Vector();
	float angleToIntent = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(forwardDir, rawIntent), -1.f, 1.f)));
	
	if (angleToIntent > DataPtr_->MaxAllowedIntentAngle)
	{
		outIntentDir = forwardDir.RotateAngleAxis(DataPtr_->MaxAllowedIntentAngle,
			FVector::CrossProduct(forwardDir, rawIntent).GetSafeNormal());
	}
	else outIntentDir = rawIntent;
}

void UPFAssistedAvoidancePreset::CalculateDynamicRays(const FVector& startPos, const FVector& forwardDir,
													const float* rayDist, FVector& outTotalRepulsion,
													FVector& outFirstOpenDir, float& outClosestDistance,
													bool& bOutHitCenter, int& outOpenRayCount)
{
	outTotalRepulsion = FVector::ZeroVector;
	outFirstOpenDir = FVector::ZeroVector;
	outClosestDistance = MAX_flt;
	bOutHitCenter = false;
	outOpenRayCount = 0;
	
	FVector rightDir = CollisionResourcePtr_->ForwardRootPtr->GetRightVector();
	FVector upDir = CollisionResourcePtr_->ForwardRootPtr->GetUpVector();
	FVector rayDirs[9] = {
		forwardDir, forwardDir.RotateAngleAxis(DataPtr_->ConeAngle, rightDir),
		forwardDir.RotateAngleAxis(-DataPtr_->ConeAngle, rightDir),
		forwardDir.RotateAngleAxis(DataPtr_->ConeAngle, upDir), forwardDir.RotateAngleAxis(-DataPtr_->ConeAngle, upDir),
		forwardDir.RotateAngleAxis(DataPtr_->ConeAngle, rightDir + upDir),
		forwardDir.RotateAngleAxis(DataPtr_->ConeAngle, -rightDir + upDir),
		forwardDir.RotateAngleAxis(-DataPtr_->ConeAngle, rightDir + upDir),
		forwardDir.RotateAngleAxis(-DataPtr_->ConeAngle, -rightDir + upDir)
	};

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());
	FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);

	for (int i = 0; i < 9; ++i)
	{
		FHitResult hit;

		bool bHit = i == 0
				? OwnerWorldPtr_->SweepSingleByChannel(hit, startPos, startPos + (rayDirs[i] * rayDist[i]),
														FQuat::Identity, ECC_Visibility, sweepShape, queryParams)
				: OwnerWorldPtr_->LineTraceSingleByChannel(hit, startPos, startPos + (rayDirs[i] * rayDist[i]),
															ECC_Visibility, queryParams);
		
		if (!bHit)
		{
			if (outOpenRayCount == 0) outFirstOpenDir = rayDirs[i];
			outOpenRayCount++;
			continue;
		}
		if (i == 0)
		{
			bOutHitCenter = true;
		}
		
		if (hit.Distance < outClosestDistance)
		{
			outClosestDistance = hit.Distance;
		}
		
		FVector effectiveNormal = hit.ImpactNormal;
		
		if (i > 0)
		{
			FVector outwardDir = (rayDirs[i] - (forwardDir * FVector::DotProduct(rayDirs[i], forwardDir))).
				GetSafeNormal();
			float outwardDot = FVector::DotProduct(effectiveNormal, outwardDir);
			if (outwardDot > 0.f)
			{
				effectiveNormal -= (2.f * outwardDot * outwardDir);
				effectiveNormal.Normalize();
			}
		}
		
		outTotalRepulsion += effectiveNormal * (1.0f - (hit.Distance / rayDist[i]));
	}
}

void UPFAssistedAvoidancePreset::JudgeAndApplyPredictiveForces(float deltaTime, const FVector& forwardDir,
																const FVector& intentDir, const FVector& totalRepulsion,
																const FVector& firstOpenDir, float closestDistance,
																float dynamicAvoidDistance, float speedMultiplier,
																bool bHitCenter, int openRayCount)
{
	if (closestDistance == MAX_flt)
	{
		CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, forwardDir, deltaTime, DataPtr_->SmoothingTurn);
		bIsInHardAvoid_ = false;
		return;
	}

	FVector hardAvoidSteering = forwardDir + totalRepulsion;
	if (bHitCenter && totalRepulsion.Length() < 0.2f)
	{
		if (openRayCount > 0)
		{
			hardAvoidSteering = firstOpenDir;
		}
		else if (!CollisionResourcePtr_->bHitRight)
		{
			hardAvoidSteering = CollisionResourcePtr_->ForwardRootPtr->GetRightVector();
		}
		else if (!CollisionResourcePtr_->bHitUp)
		{
			hardAvoidSteering = CollisionResourcePtr_->ForwardRootPtr->GetUpVector();
		}
		else if (!CollisionResourcePtr_->bHitLeft)
		{
			hardAvoidSteering = -CollisionResourcePtr_->ForwardRootPtr->GetRightVector();
		}
		else hardAvoidSteering = -CollisionResourcePtr_->ForwardRootPtr->GetUpVector();
	}
	
	hardAvoidSteering.Normalize();

	FVector finalTargetSteering = hardAvoidSteering;
	if (!totalRepulsion.IsNearlyZero())
	{
		float distScore = DataPtr_->DistanceAllowanceCurve
							? DataPtr_->DistanceAllowanceCurve->GetFloatValue(closestDistance)
							: 1.0f;
		float dirScore = DataPtr_->DirectionAllowanceCurve
							? DataPtr_->DirectionAllowanceCurve->GetFloatValue(
								FVector::DotProduct(intentDir, totalRepulsion.GetSafeNormal()))
							: 1.0f;
		if (FVector::DotProduct(intentDir, totalRepulsion.GetSafeNormal()) > 0.5f || (distScore * dirScore) >= DataPtr_
			->IntentAcceptanceThreshold)
		{
			finalTargetSteering = intentDir;
		}
	}
	else finalTargetSteering = intentDir;

	CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, finalTargetSteering, deltaTime, DataPtr_->SmoothingTurn);
	if (closestDistance < DataPtr_->AvoidDistance)
		bIsInHardAvoid_ = true;
	else if (closestDistance > dynamicAvoidDistance * 1.15f)
		bIsInHardAvoid_ = false;

	if (bIsInHardAvoid_)
	{
		PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAvoid * deltaTime, false);
		PhysicResourcePtr_->AddVelocity(CurrentRepulsion_ * DataPtr_->AvoidForce);
		return;
	}

	if (!DataPtr_->AssistForceCurve || !DataPtr_->AssistTurnCurve)
		return;
	
	float intensity = FMath::Clamp(
		1.0f - ((closestDistance - DataPtr_->AvoidDistance) / (DataPtr_->AssistDistance - DataPtr_->AvoidDistance)),
		0.0f, 1.0f);
	
	PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAssist * deltaTime, false);
	PhysicResourcePtr_->AddVelocity(
		CurrentRepulsion_ * (DataPtr_->AssistForce * DataPtr_->AssistForceCurve->GetFloatValue(intensity) *
			speedMultiplier));
	
	float turnMult = DataPtr_->AssistTurnCurve->GetFloatValue(intensity) * DataPtr_->AssistTurnSpeed * speedMultiplier;

	PhysicResourcePtr_->AddAssistPitch(FMath::FindDeltaAngleDegrees(
		CollisionResourcePtr_->ForwardRootPtr->GetComponentRotation().Pitch,
		CurrentRepulsion_.Rotation().Pitch) * turnMult);

	PhysicResourcePtr_->SetYawRotationVelocity(
		FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw,
									CurrentRepulsion_.Rotation().Yaw) * turnMult);
}

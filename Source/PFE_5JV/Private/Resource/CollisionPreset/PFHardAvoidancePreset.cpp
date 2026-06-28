#include "Resource/CollisionPreset/PFHardAvoidancePreset.h"
#include "Resource/PFCollisionResource.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFHardAvoidancePreset::InitPreset_Implementation(UPFCollisionResource* collisionResource)
{
    Super::InitPreset_Implementation(collisionResource);

    if (OwnerPtr_)
    {
        PhysicResourcePtr_ = OwnerPtr_->GetStateComponent<UPFPhysicResource>();
        OwnerWorldPtr_ = OwnerPtr_->GetWorld();
    }
}

void UPFHardAvoidancePreset::OnTickActions_Implementation(float deltaTime)
{
    Super::OnTickActions_Implementation(deltaTime);

    if (!DataPtr_ || !PhysicResourcePtr_ || !CollisionResourcePtr_ || !OwnerWorldPtr_)
    {
        return;
    }

    FVector currentVelocity = PhysicResourcePtr_->GetCurrentVelocity();
    PerformBlindRepulsion(deltaTime, currentVelocity);
}

void UPFHardAvoidancePreset::PerformBlindRepulsion(float deltaTime, const FVector& currentVelocity)
{
    FVector startPos = CollisionResourcePtr_->PhysicRoot->GetComponentLocation();
    FVector forwardDir = CollisionResourcePtr_->ForwardRootPtr->GetForwardVector();

    if (CheckAbsoluteShield(deltaTime, currentVelocity, startPos, forwardDir))
    {
        return;
    }

    float distanceThisFrame = currentVelocity.Length() * deltaTime;
    float speedPercentage = PhysicResourcePtr_->GetForwardVelocityPercentage();
    
    if (DataPtr_->AssistSpeedMultiplierCurve)
    {
        speedPercentage = DataPtr_->AssistSpeedMultiplierCurve->GetFloatValue(speedPercentage);
    }
        
    float speedMultiplier = FMath::Lerp(DataPtr_->MinSpeedMultiplier, DataPtr_->MaxSpeedMultiplier, speedPercentage);

    float dynAssist = distanceThisFrame + (DataPtr_->AssistDistance * speedMultiplier);
    float dynSides = distanceThisFrame + (DataPtr_->AssistDistanceSides * speedMultiplier);
    float dynDiag = distanceThisFrame + (DataPtr_->AssistDistanceDiagonal * speedMultiplier);
    float dynAvoid = distanceThisFrame + (DataPtr_->AvoidDistance * speedMultiplier);

    float rayDist[9] = { dynAssist, dynSides, dynSides, dynSides, dynSides, dynDiag, dynDiag, dynDiag, dynDiag };

    FVector totalRepulsion;
    FVector firstOpenDir;
    float closestDistance;
    bool bHitCenter;
    int openRayCount;
    
    CalculateDynamicRays(startPos, forwardDir, rayDist, totalRepulsion, firstOpenDir, closestDistance, bHitCenter, openRayCount);
    ApplyPredictiveForces(deltaTime, forwardDir, totalRepulsion, firstOpenDir, closestDistance, dynAvoid, speedMultiplier, bHitCenter, openRayCount);
}

bool UPFHardAvoidancePreset::CheckAbsoluteShield(float deltaTime, const FVector& currentVelocity, const FVector& startPos, const FVector& forwardDir)
{
    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());
    FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);

    FHitResult criticalHit;
    FVector nextFramePos = startPos + (currentVelocity * deltaTime) + (forwardDir * DataPtr_->CriticalBrakeDistance);

    bool bCriticalCrash = OwnerWorldPtr_->SweepSingleByChannel(criticalHit, startPos, nextFramePos, FQuat::Identity, ECC_Visibility, sweepShape, queryParams);

    if (bCriticalCrash && criticalHit.Distance < DataPtr_->CriticalBrakeDistance)
    {
        FVector velocityProjected = FVector::VectorPlaneProject(currentVelocity, criticalHit.ImpactNormal);
        CollisionResourcePtr_->PhysicRoot->SetPhysicsLinearVelocity(velocityProjected);

        PhysicResourcePtr_->CurrentForwardVelocity_ = PhysicResourcePtr_->CurrentForwardVelocity_.GetSafeNormal() * velocityProjected.Length();

        if (!velocityProjected.IsNearlyZero())
        {
            FRotator slideRotation = velocityProjected.GetSafeNormal().Rotation();

            CollisionResourcePtr_->PhysicRoot->SetWorldRotation(FRotator(0.f, slideRotation.Yaw, 0.f));
            CollisionResourcePtr_->ForwardRootPtr->SetRelativeRotation(FRotator(slideRotation.Pitch, 0.f, 0.f));
            PhysicResourcePtr_->HardSetPitchRotationVisual(slideRotation.Pitch);
        }

        CurrentRepulsion_ = criticalHit.ImpactNormal;
        return true;
    }

    return false;
}

void UPFHardAvoidancePreset::CalculateDynamicRays(const FVector& startPos, const FVector& forwardDir, const float* rayDist, 
                                                  FVector& outTotalRepulsion, FVector& outFirstOpenDir, float& outClosestDistance, 
                                                  bool& bOutHitCenter, int& outOpenRayCount)
{
    outTotalRepulsion = FVector::ZeroVector;
    outFirstOpenDir = FVector::ZeroVector;
    outClosestDistance = MAX_flt;
    bOutHitCenter = false;
    outOpenRayCount = 0;

    FVector rightDir = CollisionResourcePtr_->ForwardRootPtr->GetRightVector();
    FVector upDir = CollisionResourcePtr_->ForwardRootPtr->GetUpVector();
    float angle = DataPtr_->ConeAngle;

    FVector rayDirs[9] = {
        forwardDir,
        forwardDir.RotateAngleAxis(angle, rightDir),
        forwardDir.RotateAngleAxis(-angle, rightDir),
        forwardDir.RotateAngleAxis(angle, upDir),
        forwardDir.RotateAngleAxis(-angle, upDir),
        forwardDir.RotateAngleAxis(angle, rightDir + upDir),
        forwardDir.RotateAngleAxis(angle, -rightDir + upDir),
        forwardDir.RotateAngleAxis(-angle, rightDir + upDir),
        forwardDir.RotateAngleAxis(-angle, -rightDir + upDir)
    };

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());
    FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->PreshotSphereSize * 0.5f);

    for (int i = 0; i < 9; ++i)
    {
        FHitResult hit;
        FVector endPos = startPos + (rayDirs[i] * rayDist[i]);

        bool bHit = (i == 0)
            ? OwnerWorldPtr_->SweepSingleByChannel(hit, startPos, endPos, FQuat::Identity, ECC_Visibility, sweepShape, queryParams)
            : OwnerWorldPtr_->LineTraceSingleByChannel(hit, startPos, endPos, ECC_Visibility, queryParams);

        if (!bHit)
        {
            if (outOpenRayCount == 0) outFirstOpenDir = rayDirs[i];
            outOpenRayCount++;
            continue;
        }

        if (i == 0) bOutHitCenter = true;
        if (hit.Distance < outClosestDistance) outClosestDistance = hit.Distance;

        FVector effectiveNormal = hit.ImpactNormal;

        if (i > 0)
        {
            FVector outwardDir = forwardDir * FVector::DotProduct(rayDirs[i], forwardDir);
            outwardDir = rayDirs[i] - outwardDir;
            outwardDir = outwardDir.GetSafeNormal();
            float outwardDot = FVector::DotProduct(effectiveNormal, outwardDir);

            if (outwardDot > 0.f)
            {
                effectiveNormal -= (2.f * outwardDot * outwardDir);
                effectiveNormal.Normalize();
            }
        }

        float weight = 1.0f - (hit.Distance / rayDist[i]);
        outTotalRepulsion += effectiveNormal * weight;

#if !UE_BUILD_SHIPPING
        if (DataPtr_->ShowDebug) 
        {
           DrawDebugLine(OwnerWorldPtr_, startPos, endPos, FColor::Red, false, -1.f, 0, 1.f);
        }
#endif
    }
}

void UPFHardAvoidancePreset::ApplyPredictiveForces(float deltaTime, const FVector& forwardDir, const FVector& totalRepulsion, 
                                                   const FVector& firstOpenDir, float closestDistance, float dynamicAvoidDistance, 
                                                   float speedMultiplier, bool bHitCenter, int openRayCount)
{
    if (closestDistance == MAX_flt)
    {
        CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, forwardDir, deltaTime, DataPtr_->SmoothingTurn);
        bIsInHardAvoid_ = false;
        return;
    }

    FVector targetSteering = forwardDir + totalRepulsion;

    if (bHitCenter && totalRepulsion.Length() < 0.2f)
    {
        if (openRayCount > 0) 
        {
            targetSteering = firstOpenDir;
        }
        else
        {
            FVector rightDir = CollisionResourcePtr_->ForwardRootPtr->GetRightVector();
            FVector upDir = CollisionResourcePtr_->ForwardRootPtr->GetUpVector();
            
            if (!CollisionResourcePtr_->bHitRight) targetSteering = rightDir;
            else if (!CollisionResourcePtr_->bHitUp) targetSteering = upDir;
            else if (!CollisionResourcePtr_->bHitLeft) targetSteering = -rightDir;
            else targetSteering = -upDir;
        }
    }

    targetSteering.Normalize();
    CurrentRepulsion_ = FMath::VInterpTo(CurrentRepulsion_, targetSteering, deltaTime, DataPtr_->SmoothingTurn);

    float avoidExitThreshold = dynamicAvoidDistance * 1.15f;

    if (closestDistance < DataPtr_->AvoidDistance) bIsInHardAvoid_ = true;
    else if (closestDistance > avoidExitThreshold) bIsInHardAvoid_ = false;

    if (bIsInHardAvoid_)
    {
       PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAvoid * deltaTime, false);
       PhysicResourcePtr_->AddVelocity(CurrentRepulsion_ * DataPtr_->AvoidForce);

       float targetPitchOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->ForwardRootPtr->GetComponentRotation().Pitch, CurrentRepulsion_.Rotation().Pitch);
       PhysicResourcePtr_->AddAssistPitch(targetPitchOffset * DataPtr_->AvoidForceRot);

       float targetYawOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw, CurrentRepulsion_.Rotation().Yaw);
       PhysicResourcePtr_->SetYawRotationVelocity(targetYawOffset * DataPtr_->AvoidForceRot);
       return;
    }

    if (!DataPtr_->AssistForceCurve || !DataPtr_->AssistTurnCurve) return;

    float distanceRatio = (closestDistance - DataPtr_->AvoidDistance) / (DataPtr_->AssistDistance - DataPtr_->AvoidDistance);
    float intensity = FMath::Clamp(1.0f - distanceRatio, 0.0f, 1.0f);

    PhysicResourcePtr_->AddForwardVelocity(-DataPtr_->SlowForceAssist * deltaTime, false);

    float forceMultiplier = DataPtr_->AssistForceCurve->GetFloatValue(intensity);
    PhysicResourcePtr_->AddVelocity(CurrentRepulsion_ * (DataPtr_->AssistForce * forceMultiplier * speedMultiplier));

    float turnMultiplier = DataPtr_->AssistTurnCurve->GetFloatValue(intensity);

    float targetPitchOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->ForwardRootPtr->GetComponentRotation().Pitch, CurrentRepulsion_.Rotation().Pitch);
    PhysicResourcePtr_->AddAssistPitch(targetPitchOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * speedMultiplier);

    float targetYawOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw, CurrentRepulsion_.Rotation().Yaw);
    PhysicResourcePtr_->SetYawRotationVelocity(targetYawOffset * turnMultiplier * DataPtr_->AssistTurnSpeed * speedMultiplier);
}
#include "Resource/CollisionPreset/PFAssistedAvoidancePreset.h"
#include "Resource/PFCollisionResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFAssistedAvoidancePreset::InitPreset_Implementation(UPFCollisionResource* collisionResource)
{
    Super::InitPreset_Implementation(collisionResource);

    if (OwnerPtr_)
    {
        PhysicResourcePtr_ = OwnerPtr_->GetStateComponent<UPFPhysicResource>();
        OwnerWorldPtr_ = OwnerPtr_->GetWorld();
    }
}

void UPFAssistedAvoidancePreset::OnTickActions_Implementation(float deltaTime)
{
    Super::OnTickActions_Implementation(deltaTime);

    if (!DataPtr_ || !PhysicResourcePtr_ || !CollisionResourcePtr_ || !OwnerWorldPtr_)
    {
        return;
    }

    FVector currentVelocity = PhysicResourcePtr_->GetCurrentVelocity();
    ProcessAssistedAvoidance(deltaTime, currentVelocity);
}

void UPFAssistedAvoidancePreset::ProcessAssistedAvoidance(float deltaTime, const FVector& currentVelocity)
{
    FVector startPos = CollisionResourcePtr_->PhysicRoot->GetComponentLocation();
    FVector forwardDir = CollisionResourcePtr_->ForwardRootPtr->GetForwardVector();

    if (ProcessForwardShield(startPos, forwardDir, currentVelocity))
    {
        return;
    }

    float turnMagnitude = 0.f;
    FVector intentDir = forwardDir;
    CalculatePlayerIntent(turnMagnitude, intentDir);
    ProcessIntentSphere(deltaTime, startPos, forwardDir, intentDir, turnMagnitude);
}

void UPFAssistedAvoidancePreset::CalculatePlayerIntent(float& outTurnMagnitude, FVector& outIntentDir)
{
    float totalYaw = PhysicResourcePtr_->GetYawAngularVelocity();
    float totalPitch = PhysicResourcePtr_->GetCurrentPitchRotation();

    float yawMag = FMath::Clamp(FMath::Abs(totalYaw) / DataPtr_->MaxExpectedYawVelocity, 0.f, 1.f);
    float pitchMag = FMath::Clamp(FMath::Abs(totalPitch) / DataPtr_->MaxExpectedPitchRotation, 0.f, 1.f);
    
    outTurnMagnitude = FMath::Max(yawMag, pitchMag);

    FRotator intentLocalRot(totalPitch, totalYaw, 0.f);
    outIntentDir = intentLocalRot.RotateVector(CollisionResourcePtr_->ForwardRootPtr->GetForwardVector()).GetSafeNormal();
}

bool UPFAssistedAvoidancePreset::ProcessForwardShield(const FVector& startPos, const FVector& forwardDir, const FVector& currentVelocity)
{
    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());

    FHitResult shieldHit;
    FVector endPos = startPos + (forwardDir * DataPtr_->ForwardShieldDistance);

    bool bHit = OwnerWorldPtr_->LineTraceSingleByChannel(shieldHit, startPos, endPos, ECC_Visibility, queryParams);

    if (bHit)
    {
        FVector velocityProjected = FVector::VectorPlaneProject(currentVelocity, shieldHit.ImpactNormal);
        CollisionResourcePtr_->PhysicRoot->SetPhysicsLinearVelocity(velocityProjected);
        PhysicResourcePtr_->CurrentForwardVelocity_ = PhysicResourcePtr_->CurrentForwardVelocity_.GetSafeNormal() * velocityProjected.Length();

#if !UE_BUILD_SHIPPING
        if (DataPtr_->bShowDebug) DrawDebugLine(OwnerWorldPtr_, startPos, endPos, FColor::Red, false, -1.f, 0, 2.f);
#endif
        return true;
    }

#if !UE_BUILD_SHIPPING
    if (DataPtr_->bShowDebug) DrawDebugLine(OwnerWorldPtr_, startPos, endPos, FColor::Green, false, -1.f, 0, 1.f);
#endif
    return false;
}

void UPFAssistedAvoidancePreset::ProcessIntentSphere(float deltaTime, const FVector& startPos, const FVector& forwardDir, const FVector& intentDir, float turnMagnitude)
{
    float blendAlpha = DataPtr_->RayDirectionBlendCurve ? DataPtr_->RayDirectionBlendCurve->GetFloatValue(turnMagnitude) : turnMagnitude;
    FVector rayDir = FMath::Lerp(forwardDir, intentDir, blendAlpha).GetSafeNormal();

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());
    FCollisionShape sweepShape = FCollisionShape::MakeSphere(DataPtr_->IntentSphereRadius);

    FHitResult sphereHit;
    FVector endPos = startPos + (rayDir * DataPtr_->IntentSphereDistance);

    bool bHit = OwnerWorldPtr_->SweepSingleByChannel(sphereHit, startPos, endPos, FQuat::Identity, ECC_Visibility, sweepShape, queryParams);

    if (bHit)
    {
        float proximityRatio = 1.0f - FMath::Clamp(sphereHit.Distance / DataPtr_->IntentSphereDistance, 0.0f, 1.0f);

        float forceMultiplier = DataPtr_->AvoidanceForceCurve ? DataPtr_->AvoidanceForceCurve->GetFloatValue(proximityRatio) : proximityRatio;

        float splitAlpha = DataPtr_->TurnPropulsionSplitCurve ? DataPtr_->TurnPropulsionSplitCurve->GetFloatValue(turnMagnitude) : turnMagnitude;

        FVector targetSteering = FMath::Lerp(sphereHit.ImpactNormal, intentDir, splitAlpha).GetSafeNormal();

        float targetPitchOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->ForwardRootPtr->GetComponentRotation().Pitch, targetSteering.Rotation().Pitch);
        float targetYawOffset = FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw, targetSteering.Rotation().Yaw);

        PhysicResourcePtr_->AddAssistPitch(targetPitchOffset * forceMultiplier * DataPtr_->BasePitchAssist);
        PhysicResourcePtr_->SetYawRotationVelocity(targetYawOffset * forceMultiplier * DataPtr_->BaseYawAssist);

        if (splitAlpha > 0.5f)
        {
            FVector propelForce = targetSteering * (DataPtr_->TurnPropelForce * forceMultiplier * deltaTime);
            PhysicResourcePtr_->AddVelocity(propelForce, false, false, 0.f, nullptr);
        }

#if !UE_BUILD_SHIPPING
        if (DataPtr_->bShowDebug) 
        {
            DrawDebugLine(OwnerWorldPtr_, startPos, sphereHit.ImpactPoint, FColor::Orange, false, -1.f, 0, 3.f);
            DrawDebugLine(OwnerWorldPtr_, sphereHit.ImpactPoint, sphereHit.ImpactPoint + (targetSteering * 500.f), FColor::Cyan, false, -1.f, 0, 5.f);
        }
#endif
    }
}
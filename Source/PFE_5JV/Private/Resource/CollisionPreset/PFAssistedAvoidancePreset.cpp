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
		UE_LOG(LogTemp, Error, TEXT("[AssistedAvoidancePreset] Error In set up, either couldn't get a var or data is null"))
		return;
	}

	ProcessAssistedAvoidance(deltaTime, PhysicResourcePtr_->GetCurrentVelocity());
}

void UPFAssistedAvoidancePreset::ProcessAssistedAvoidance(float deltaTime, const FVector& currentVelocity)
{
    FVector startPos = CollisionResourcePtr_->PhysicRoot->GetComponentLocation();
    FVector intentDir;
    CalculatePlayerIntent(intentDir);

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(CollisionResourcePtr_->GetOwner());

    FHitResult CenterHit;
    bool bCenterHit = OwnerWorldPtr_->SweepSingleByChannel(CenterHit, startPos, startPos + (intentDir * DataPtr_->IntentSphereDistance), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(DataPtr_->IntentSphereRadius), queryParams);

    if (!bCenterHit)
    {
        SmoothedSteering_ = FMath::VInterpTo(SmoothedSteering_, FVector::ZeroVector, deltaTime, DataPtr_->SmoothingSpeed);
        return;
    }

    FRotationMatrix IntentMatrix(intentDir.Rotation());
    FVector Right = IntentMatrix.GetScaledAxis(EAxis::Y);
    FVector Up = IntentMatrix.GetScaledAxis(EAxis::Z);
    FVector WhiskerDirs[4] = { (intentDir + Right * DataPtr_->WhiskerSpread).GetSafeNormal(), (intentDir - Right * DataPtr_->WhiskerSpread).GetSafeNormal(), (intentDir + Up * DataPtr_->WhiskerSpread).GetSafeNormal(), (intentDir - Up * DataPtr_->WhiskerSpread).GetSafeNormal() };

    for (int i = 0; i < 4; i++)
    {
        FHitResult WhiskerHit;
        if (!OwnerWorldPtr_->LineTraceSingleByChannel(WhiskerHit, startPos, startPos + (WhiskerDirs[i] * DataPtr_->IntentSphereDistance), ECC_Visibility, queryParams))
        {
            ApplySmoothSteering(WhiskerDirs[i], deltaTime);
            return;
        }
    }

    FVector SkimDir = FVector::VectorPlaneProject(intentDir, CenterHit.ImpactNormal).GetSafeNormal();
    if (SkimDir.IsNearlyZero()) SkimDir = CenterHit.ImpactNormal;
    ApplySmoothSteering(SkimDir, deltaTime);
}

void UPFAssistedAvoidancePreset::CalculatePlayerIntent(FVector& outIntentDir)
{
    float totalYaw = PhysicResourcePtr_->GetYawAngularVelocity();
    float targetPitch = PhysicResourcePtr_->GetCurrentPitchRotation();
    float currentYaw = CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw;
    outIntentDir = FRotator(targetPitch, currentYaw + totalYaw, 0.f).Vector();
}

void UPFAssistedAvoidancePreset::ApplySmoothSteering(const FVector& targetDir, float deltaTime)
{
    SmoothedSteering_ = FMath::VInterpTo(SmoothedSteering_, targetDir, deltaTime, DataPtr_->SmoothingSpeed);
    float intensity = SmoothedSteering_.Length();
    if (intensity > 0.01f)
    {
        FVector steerDir = SmoothedSteering_.GetSafeNormal();
        FRotator steerRot = steerDir.Rotation();
        FRotator currentRot = CollisionResourcePtr_->ForwardRootPtr->GetComponentRotation();
        
        PhysicResourcePtr_->AddAssistPitch(FMath::FindDeltaAngleDegrees(currentRot.Pitch, steerRot.Pitch) * DataPtr_->BasePitchAssist * intensity);
        PhysicResourcePtr_->SetYawRotationVelocity(FMath::FindDeltaAngleDegrees(CollisionResourcePtr_->PhysicRoot->GetComponentRotation().Yaw, steerRot.Yaw) * DataPtr_->BaseYawAssist * intensity);
        PhysicResourcePtr_->AddVelocity(steerDir * (DataPtr_->TurnPropelForce * intensity * deltaTime), false, false, 0.f, nullptr);
    }
}
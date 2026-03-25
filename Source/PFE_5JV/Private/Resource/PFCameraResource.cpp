#include "Resource/PFCameraResource.h"
#include <algorithm>
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "Helpers/PFMathHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFCameraResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
    Super::ComponentInit_Implementation(ownerObj);

    PhysicReferencePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
    TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
    DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();

    if (!CheckValidity())
        return;

    CameraRootPtr_->SetUsingAbsoluteRotation(true);
    
    // Initialize bases to prevent starting snap
    BaseLocation_ = CameraRootPtr_->GetComponentLocation();
    BaseYaw_ = Owner->GetActorRotation().Yaw;
    BasePitch_ = Owner->GetActorRotation().Pitch;
    PreviousPlayerYaw_ = BaseYaw_;
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CheckValidity())
        return;

    // 1. Process States
    UpdateCameraDive(deltaTime);

    // 2. Base Transforms Calculations
    UpdateCameraDistance(deltaTime);
    UpdateCameraYaw(deltaTime);
    UpdateCameraPitch(deltaTime);
    UpdateCameraRoll(deltaTime);

    // 3. Procedural Additions
    ApplyProceduralEffects(deltaTime);

    // 4. Apply to Gimbal Rig
    CameraRootPtr_->SetWorldLocation(BaseLocation_);
    
    // Yaw uses World Rotation. We add Overshoot and Shake here.
    CameraYawPtr_->SetWorldRotation(FRotator(0.f, BaseYaw_ + SmoothedOvershootYaw_ + CurrentShakeYaw_, 0.f));
    
    // Pitch uses Relative Rotation. We add Shake here.
    CameraPitchPtr_->SetRelativeRotation(FRotator(BasePitch_ + CurrentShakePitch_, 0.f, 0.f));
    
    // Roll uses Relative Rotation.
    CameraRollPtr_->SetRelativeRotation(FRotator(0.f, 0.f, BaseRoll_));
}

bool UPFCameraResource::CheckValidity() const
{
    if(!CameraPtr_ || !CameraRootPtr_ || !CameraYawPtr_ || !CameraPitchPtr_ || !CameraRollPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("One or more Gimbal Components are NULL in UPFCameraResource"));
        return false;
    }
    if(!DataPtr_ || !PhysicReferencePtr_ || !TurnAbilityPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("Missing Data or Ability references in UPFCameraResource"));
        return false;
    }
    return true;
}

void UPFCameraResource::UpdateCameraDive(float DeltaTime)
{
    if (DiveAbilityPtr_ && DiveAbilityPtr_->IsActive())
    {
        ElapsedDiveTime_ += DeltaTime;
        if (ElapsedDiveTime_ >= DataPtr_->DurationBeforeDive)
        {
            IsCurrentlyDiving_ = true;
        }
    }
    else
    {
        IsCurrentlyDiving_ = false;
        ElapsedDiveTime_ = 0.f;
    }
}

void UPFCameraResource::UpdateCameraDistance(float DeltaTime)
{
    FVector TargetBaseLocation = TrackedTargetPtr_->GetComponentLocation();
    float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    
    // 1. Calculate ideal target distance
    float TargetDistance = UPFMathHelper::RemapClamped(
        CurrentSpeed,
        GlideAbilityDataPtr_->AutoDiveSpeedLimit,
        PhysicReferencePtr_->GetMaxBoostVelocity(),
        DataPtr_->MinDistanceToCamera * 100.f,
        DataPtr_->MaxDistanceToCamera * 100.f);
    
    // 2. Use the Tracked Target's backward vector
    FVector Backward = -TrackedTargetPtr_->GetForwardVector();
    FVector TargetLocation = TargetBaseLocation + (Backward * TargetDistance) + DataPtr_->BaseOffset;
    
    if (IsCurrentlyDiving_)
    {
        TargetLocation += DataPtr_->DiveOffset; 
    }

    // 3. Apply Delay
    BaseLocation_ = FMath::VInterpTo(BaseLocation_, TargetLocation, DeltaTime, DataPtr_->DistanceInterpSpeed);

    // 4. Hard Clamp Distance relative to the Tracked Target
    FVector OffsetFromTarget = BaseLocation_ - TargetBaseLocation;
    float CurrentDistance = OffsetFromTarget.Size();
    float MinDist = DataPtr_->MinDistanceToCamera * 100.f;
    float MaxDist = DataPtr_->MaxDistanceToCamera * 100.f;
    
    if (CurrentDistance > MaxDist)
    {
        BaseLocation_ = TargetBaseLocation + (OffsetFromTarget / CurrentDistance) * MaxDist;
    }
    else if (CurrentDistance < MinDist && CurrentDistance > 0.1f)
    {
        BaseLocation_ = TargetBaseLocation + (OffsetFromTarget / CurrentDistance) * MinDist;
    }
}

void UPFCameraResource::UpdateCameraYaw(float DeltaTime)
{
    float TargetYaw = TrackedTargetPtr_->GetComponentRotation().Yaw;
    
    // 1. Apply Delay (Lag) copying the target's yaw
    BaseYaw_ = FMath::RInterpTo(FRotator(0.f, BaseYaw_, 0.f), FRotator(0.f, TargetYaw, 0.f), DeltaTime, DataPtr_->YawLagSpeed).Yaw;

    // 2. Clamp Base Yaw to Target Yaw so it doesn't fall too far behind
    float DeltaYaw = FMath::FindDeltaAngleDegrees(TargetYaw, BaseYaw_);
    float ClampedDeltaYaw = FMath::Clamp(DeltaYaw, -DataPtr_->MaxYawAngle, DataPtr_->MaxYawAngle);
    
    BaseYaw_ = TargetYaw + ClampedDeltaYaw;
}

void UPFCameraResource::UpdateCameraPitch(float DeltaTime)
{
    float TargetPitch = TrackedTargetPtr_->GetComponentRotation().Pitch;
    
    if (IsCurrentlyDiving_)
    {
        TargetPitch += DataPtr_->DivePitchRotation;
    }

    // 1. Apply Delay (Lag) copying the target's pitch
    BasePitch_ = FMath::RInterpTo(FRotator(BasePitch_, 0.f, 0.f), FRotator(TargetPitch, 0.f, 0.f), DeltaTime, DataPtr_->PitchLagSpeed).Pitch;

    // 2. Clamp Base Pitch to Target Pitch
    float DeltaPitch = FMath::FindDeltaAngleDegrees(TargetPitch, BasePitch_);
    float ClampedDeltaPitch = FMath::Clamp(DeltaPitch, -DataPtr_->MaxPitchAngle, DataPtr_->MaxPitchAngle);

    BasePitch_ = TargetPitch + ClampedDeltaPitch;
}

void UPFCameraResource::UpdateCameraRoll(float DeltaTime)
{
    float CurrentPlayerYaw = Owner->GetActorRotation().Yaw;
    float YawTurnRate = FMath::FindDeltaAngleDegrees(PreviousPlayerYaw_, CurrentPlayerYaw) / DeltaTime;
    
    // Calculate Roll based on turn rate
    float TargetRoll = FMath::Clamp(YawTurnRate * 0.15f, -DataPtr_->RollAmplitude, DataPtr_->RollAmplitude);

    BaseRoll_ = FMath::FInterpTo(BaseRoll_, TargetRoll, DeltaTime, DataPtr_->LookAtInterpSpeed);
    
    PreviousPlayerYaw_ = CurrentPlayerYaw;
}

void UPFCameraResource::ApplyProceduralEffects(float DeltaTime)
{
    float TimeSeconds = Owner->GetWorld()->GetTimeSeconds();
    float CurrentPlayerYaw = Owner->GetActorRotation().Yaw;
    float YawTurnRate = FMath::FindDeltaAngleDegrees(PreviousPlayerYaw_, CurrentPlayerYaw) / DeltaTime;

    // Overshoot
    float TargetOvershoot = FMath::Clamp(-YawTurnRate * 0.05f, -DataPtr_->OvershootYawOffset, DataPtr_->OvershootYawOffset);
    SmoothedOvershootYaw_ = FMath::FInterpTo(SmoothedOvershootYaw_, TargetOvershoot, DeltaTime, DataPtr_->OvershootInterpSpeed);

    // Shake
    float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    if (CurrentSpeed > GlideAbilityDataPtr_->AutoDiveSpeedLimit || IsCurrentlyDiving_)
    {
        CurrentShakePitch_ = FMath::Sin(TimeSeconds * DataPtr_->ShakeFrequency) * DataPtr_->PitchAmplitude * DeltaTime;
        CurrentShakeYaw_ = FMath::Cos(TimeSeconds * DataPtr_->ShakeFrequency * 1.2f) * DataPtr_->YawAmplitude * DeltaTime;
    }
    else
    {
        // Smoothly dampen shake to 0 when slowing down
        CurrentShakePitch_ = FMath::FInterpTo(CurrentShakePitch_, 0.f, DeltaTime, 10.f);
        CurrentShakeYaw_ = FMath::FInterpTo(CurrentShakeYaw_, 0.f, DeltaTime, 10.f);
    }
}
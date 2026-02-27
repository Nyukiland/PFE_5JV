#include "Resource/PFCameraResource.h"

#include <algorithm>
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "Helpers/PFMathHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "StateMachine/PFPlayerCharacter.h"

UPFCameraResource::UPFCameraResource()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UPFCameraResource::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
    Super::ComponentInit_Implementation(ownerObj);

    PhysicReferencePtr_ = Owner->GetStateComponent<UPFPhysicResource>();
    TurnAbilityPtr_ = Owner->GetStateComponent<UPFTurnAbility>();
    DiveAbilityPtr_ = Owner->GetStateComponent<UPFDiveAbility>();

    if (!CheckValidity())
        return;

    CameraRootPtr_->SetUsingAbsoluteRotation(true);
    PreviousYaw_ = Owner->GetActorRotation().Yaw;
    BaseCameraRoll_ = CameraPtr_->GetRelativeRotation().Roll;
    SmoothedCameraRotation_ = FRotator(PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch,
        Owner->GetActorRotation().Yaw, VisualResourcePtr_->GetRelativeRotation().Roll);
    CurrentTurnRoll_ = 0.f;
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CheckValidity())
        return;

    FRotator FinalRootRotation;

    // Camera Root movements
    UpdateCameraRotation(deltaTime, FinalRootRotation);
    UpdateCameraShake(deltaTime);

    CameraRootPtr_->SetWorldRotation(FinalRootRotation);

    // Camera movements
    UpdateCameraDistance(deltaTime);
}

bool UPFCameraResource::CheckValidity() const
{
    if(!CameraPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The Camera referenced in PlayerCharacter blueprint is NULL"))
        return false;
    }

    if(!CameraRootPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The CameraRoot referenced in PlayerCharacter blueprint is NULL"))
        return false;
    }

    if(!DataPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The Data referenced in PlayerCharacter blueprint is NULL"))
        return false;
    }

    if(!PhysicReferencePtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The PhysicResource referenced in PlayerCharacter blueprint is NULL"))
        return false;
    }

    if(!VisualResourcePtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The VisualResource referenced in PlayerCharacter blueprint is NULL"))
        return false;
    }

    if(!TurnAbilityPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("The TurnAbility referenced in PlayerCharacter blueprint is NULL"))
        return false; 
    }

    return true;
}

void UPFCameraResource::UpdateCameraRotation(float DeltaTime, FRotator& FinalRotation)
{    
    // ---- YAW ----
    const float BaseYaw = Owner->GetActorRotation().Yaw;
    float InputYaw = TurnAbilityPtr_->InputRight_ - TurnAbilityPtr_->InputLeft_;
    float OffsetYaw = InputYaw * DataPtr_->CameraYawInputOffset;
    float TargetWorldYaw = BaseYaw + CameraYawOffset_ + OffsetYaw;
    FRotator CurrentRotYaw(0.f, SmoothedCameraRotation_.Yaw, 0.f);
    FRotator TargetRotYaw(0.f, TargetWorldYaw, 0.f);
    SmoothedCameraRotation_.Yaw = FMath::RInterpTo(CurrentRotYaw, TargetRotYaw, DeltaTime, DataPtr_->YawLagSpeed).Yaw;
    float LagDeltaYaw = FMath::FindDeltaAngleDegrees(BaseYaw, SmoothedCameraRotation_.Yaw);
    LagDeltaYaw = FMath::Clamp(LagDeltaYaw,-DataPtr_->MaxYawAngle, DataPtr_->MaxYawAngle);
    SmoothedCameraRotation_.Yaw = BaseYaw + LagDeltaYaw;
    FinalRotation.Yaw = SmoothedCameraRotation_.Yaw;
    
    // ---- PITCH ----
    const float BasePitch = PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch;
    float TargetWorldPitch = CameraPitchOffset_;
    FRotator CurrentRotPitch(SmoothedCameraRotation_.Pitch, 0.f, 0.f);
    FRotator TargetRotPitch(TargetWorldPitch, 0.f, 0.f);
    SmoothedCameraRotation_.Pitch = FMath::RInterpTo(CurrentRotPitch, TargetRotPitch, DeltaTime, DataPtr_->PitchLagSpeed).Pitch;
    float LagDeltaPitch = FMath::FindDeltaAngleDegrees(BasePitch, SmoothedCameraRotation_.Pitch);
    LagDeltaPitch = FMath::Clamp(LagDeltaPitch,-DataPtr_->MaxPitchAngle, DataPtr_->MaxPitchAngle);
    SmoothedCameraRotation_.Pitch = BasePitch + LagDeltaPitch;
    FinalRotation.Pitch = SmoothedCameraRotation_.Pitch;
    
    
    // ---- ROLL ----
    const float BaseRoll = VisualResourcePtr_->GetRelativeRotation().Roll;
    float TargetWorldRoll = CameraRollOffset_;
    FRotator CurrentRotRoll(0.0f, 0.f, SmoothedCameraRotation_.Roll);
    FRotator TargetRotRoll(0.0f, 0.f, TargetWorldRoll);
    SmoothedCameraRotation_.Roll = FMath::RInterpTo(CurrentRotRoll, TargetRotRoll, DeltaTime, DataPtr_->RollLagSpeed).Roll;
    float LagDeltaRoll = FMath::FindDeltaAngleDegrees(BaseRoll, SmoothedCameraRotation_.Roll);
    LagDeltaRoll = FMath::Clamp(LagDeltaRoll,-DataPtr_->MaxRollAngle, DataPtr_->MaxRollAngle);
    SmoothedCameraRotation_.Roll = BaseRoll + LagDeltaRoll;
    FinalRotation.Roll = SmoothedCameraRotation_.Roll;

    // ---- DEBUG ----
    float DeltaYaw = FMath::FindDeltaAngleDegrees(Owner->GetActorRotation().Yaw, FinalRotation.Yaw);
    float DeltaPitch = FMath::FindDeltaAngleDegrees(PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch, FinalRotation.Pitch);
    float DeltaRoll = FMath::FindDeltaAngleDegrees(VisualResourcePtr_->GetRelativeRotation().Roll, FinalRotation.Roll);
    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Caméra: %f/%f/%f"), CameraPtr_->GetComponentRotation().Yaw, CameraPtr_->GetComponentRotation().Pitch, CameraPtr_->GetComponentRotation().Roll));
    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Oiseau: %f/%f/%f"), DeltaYaw, DeltaPitch, DeltaRoll));
}

void UPFCameraResource::UpdateCameraShake(float DeltaTime)
{
    if (!DiveAbilityPtr_->IsDiving())
        return;

    ShakeTime_ += DeltaTime;

    float NoiseRoll  = FMath::PerlinNoise1D(ShakeTime_ * DataPtr_->ShakeFrequency);
    float NoisePitch = FMath::PerlinNoise1D((ShakeTime_ + 100.f) * DataPtr_->ShakeFrequency);
    float NoiseYaw   = FMath::PerlinNoise1D((ShakeTime_ + 200.f) * DataPtr_->ShakeFrequency);

    FRotator ShakeOffset;
    ShakeOffset.Roll  = NoiseRoll  * DataPtr_->RollAmplitude;
    ShakeOffset.Pitch = NoisePitch * DataPtr_->PitchAmplitude;
    ShakeOffset.Yaw   = NoiseYaw   * DataPtr_->YawAmplitude;

    FRotator RelativeRotation = CameraPtr_->GetRelativeRotation();
    CameraPtr_->SetRelativeRotation(RelativeRotation + ShakeOffset);
}

void UPFCameraResource::UpdateCameraDistance(float DeltaTime)
{
    const float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    const float CameraDistance = UPFMathHelper::RemapClamped(
            CurrentSpeed,
            GlideAbilityDataPtr_->AutoDiveSpeedLimit,
            PhysicReferencePtr_->GetMaxSpeed(),
            DataPtr_->MinDistanceToCamera,
            DataPtr_->MaxDistanceToCamera);

    FVector PlayerCenter = Owner->GetActorLocation() + FVector(0.f, 0.f, DataPtr_->LookAtHeightOffset);
    FVector Direction = CameraPtr_->GetForwardVector();
    Direction.Normalize();
    FVector TargetLocation = PlayerCenter - Direction * CameraDistance;
    FVector NewLocation = FMath::VInterpTo(CameraPtr_->GetComponentLocation(), TargetLocation, DeltaTime, DataPtr_->DistanceInterpSpeed);
    CameraPtr_->SetWorldLocation(NewLocation);
}
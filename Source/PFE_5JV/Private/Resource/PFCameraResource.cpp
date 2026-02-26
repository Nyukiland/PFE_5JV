#include "Resource/PFCameraResource.h"

#include <algorithm>
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "Helpers/PFMathHelper.h"
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

    if(!CameraPtr_)
        UE_LOG(LogTemp, Error, TEXT("The Camera referenced in PlayerCharacter blueprint is NULL"))

    if(!CameraRootPtr_)
        UE_LOG(LogTemp, Error, TEXT("The CameraRoot referenced in PlayerCharacter blueprint is NULL"))

    if(!DataPtr_)
        UE_LOG(LogTemp, Error, TEXT("The Data referenced in PlayerCharacter blueprint is NULL"))

    if(!PhysicReferencePtr_)
        UE_LOG(LogTemp, Error, TEXT("The PhysicResource referenced in PlayerCharacter blueprint is NULL"))

    if(!VisualResourcePtr_)
        UE_LOG(LogTemp, Error, TEXT("The VisualResource referenced in PlayerCharacter blueprint is NULL"))

    if(!TurnAbilityPtr_)
        UE_LOG(LogTemp, Error, TEXT("The TurnAbility referenced in PlayerCharacter blueprint is NULL"))

    CameraRootPtr_->SetUsingAbsoluteRotation(true);
    PreviousYaw_ = Owner->GetActorRotation().Yaw;
    SmoothedCameraRotation_ = Owner->GetActorRotation();
    BaseCameraRoll_ = CameraPtr_->GetRelativeRotation().Roll;
    CurrentTurnRoll_ = 0.f;
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CameraRootPtr_ || !CameraPtr_ || !DataPtr_ || !PhysicReferencePtr_ || !VisualResourcePtr_ || !TurnAbilityPtr_)
        return;

    FRotator FinalRootRotation = SmoothedCameraRotation_;

    // Camera Root movements
    UpdateCameraRotation(deltaTime, FinalRootRotation);
    // UpdateCameraShake(deltaTime, FinalRootRotation);

    CameraRootPtr_->SetWorldRotation(FinalRootRotation);
    SmoothedCameraRotation_ = FinalRootRotation;

    // Camera movements
    UpdateCameraDistance(deltaTime);
    // UpdateTurningRoll(deltaTime);
}

void UPFCameraResource::UpdateCameraRotation(float DeltaTime, FRotator& FinalRotation)
{    
    // ---- YAW ----
    const float BaseYaw = Owner->GetActorRotation().Yaw;
    float InputYaw = TurnAbilityPtr_->InputRight_ - TurnAbilityPtr_->InputLeft_;
    float OffsetYaw = InputYaw * DataPtr_->CameraYawInputOffset;
    float TargetYaw = BaseYaw + CameraYawOffset_ + OffsetYaw;
    float InterpedYaw = FMath::FInterpTo(FinalRotation.Yaw,TargetYaw,DeltaTime,DataPtr_->YawLagSpeed_);
    float RelativeYaw = FMath::FindDeltaAngleDegrees(BaseYaw, InterpedYaw);
    RelativeYaw = FMath::Clamp(RelativeYaw,-DataPtr_->MaxYawAngle ,DataPtr_->MaxYawAngle);
    float FinalYaw = BaseYaw + RelativeYaw;

    // ---- PITCH ----
    float BasePitch = PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch;
    float TargetPitch = BasePitch + CameraPitchOffset_;
    float InterpedPitch = FMath::FInterpTo(FinalRotation.Pitch,TargetPitch,DeltaTime,DataPtr_->PitchLagSpeed_);
    float RelativePitch = FMath::FindDeltaAngleDegrees(BasePitch, InterpedPitch);
    float ClampedRelativePitch = FMath::Clamp(RelativePitch,-DataPtr_->MaxPitchAngle,DataPtr_->MaxPitchAngle);
    float FinalPitch = BasePitch + ClampedRelativePitch;
    
    // ---- ROLL ----
    float BaseRoll = VisualResourcePtr_->GetRelativeRotation().Roll;
    float InterpedRoll = FMath::FInterpTo(FinalRotation.Roll,BaseRoll,DeltaTime, DataPtr_->RollLagSpeed_);
    float RelativeRoll = FMath::FindDeltaAngleDegrees(BaseRoll, InterpedRoll);
    float ClampedRelativeRoll = FMath::Clamp(RelativeRoll,-DataPtr_->MaxRollAngle,DataPtr_->MaxRollAngle);
    float FinalRoll = BaseRoll + ClampedRelativeRoll;

    // ---- APPLY ----
    FinalRotation = FRotator(FinalPitch, FinalYaw, FinalRoll);

    // ---- DEBUG ----
    float DeltaYaw = FMath::FindDeltaAngleDegrees(Owner->GetActorRotation().Yaw, FinalRotation.Yaw);
    float DeltaPitch = FMath::FindDeltaAngleDegrees(PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch, FinalRotation.Pitch);
    float DeltaRoll = FMath::FindDeltaAngleDegrees(VisualResourcePtr_->GetRelativeRotation().Roll, FinalRotation.Roll);
    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Caméra: %f/%f/%f"), CameraPtr_->GetComponentRotation().Yaw, CameraPtr_->GetComponentRotation().Pitch, CameraPtr_->GetComponentRotation().Roll));
    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Oiseau: %f/%f/%f"), DeltaYaw, DeltaPitch, DeltaRoll));
}

void UPFCameraResource::UpdateTurningRoll(float DeltaTime)
{
    float InputYaw = TurnAbilityPtr_->InputRight_ - TurnAbilityPtr_->InputLeft_;
    float TargetRoll = BaseCameraRoll_ + InputYaw * DataPtr_->MaxTurnRoll;
    
    float NewRoll = FMath::FInterpTo(CameraPtr_->GetRelativeRotation().Roll, TargetRoll, DeltaTime, DataPtr_->TurnRollSpeed);
    NewRoll = FMath::Clamp(NewRoll, BaseCameraRoll_ - DataPtr_->MaxTurnRoll, BaseCameraRoll_ + DataPtr_->MaxTurnRoll);

    FRotator RelativeRotation = CameraPtr_->GetRelativeRotation();
    RelativeRotation.Roll = NewRoll;
    CameraPtr_->SetRelativeRotation(RelativeRotation);
}

void UPFCameraResource::UpdateCameraShake(float DeltaTime, FRotator& FinalRotation)
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

    FinalRotation += ShakeOffset;
}

void UPFCameraResource::UpdateCameraDistance(float DeltaTime)
{
    const float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    const float MaxSpeed = PhysicReferencePtr_->GetMaxSpeed();
    const float MaxDistance = DataPtr_->MaxDistanceToCamera;
    const float CameraDistance = UPFMathHelper::RemapClamped(CurrentSpeed, 0.0f, MaxSpeed, 0.0f, MaxDistance);

    const FVector BaseLocation = FVector::ZeroVector;
    const FVector TargetLocation = BaseLocation + FVector(-CameraDistance, 0.f, 0.f);
    const FVector NewLocation = FMath::VInterpTo(CameraPtr_->GetRelativeLocation(),TargetLocation,DeltaTime,DataPtr_->DistanceInterpSpeed);
    CameraPtr_->SetRelativeLocation(NewLocation);
    
    // GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Speed : %f/%f => Distance : %f/%f"), CurrentSpeed, MaxSpeed, CameraDistance, MaxDistance));
}
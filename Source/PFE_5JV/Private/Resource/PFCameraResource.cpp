#include "Resource/PFCameraResource.h"

#include "EnhancedInputComponent.h"
#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
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

    FRotator FinalRotation = SmoothedCameraRotation_;
    
    UpdateCameraRotation(deltaTime, FinalRotation);
    // UpdateCameraShake(deltaTime, FinalRotation);

    SmoothedCameraRotation_ = FinalRotation;
    CameraRootPtr_->SetWorldRotation(FinalRotation);

    UpdateCameraDistance(deltaTime);
    UpdateTurningRoll(deltaTime);
}

void UPFCameraResource::UpdateCameraRotation(float DeltaTime, FRotator& FinalRotation)
{
    float YawRotation = Owner->GetActorRotation().Yaw;
    float PitchRotation = PhysicReferencePtr_->ForwardRoot->GetComponentRotation().Pitch;
    float RollRotation = VisualResourcePtr_->GetRelativeRotation().Roll;
    FRotator CameraRotation = FinalRotation;
    
    float DeltaYaw = FMath::FindDeltaAngleDegrees(YawRotation, CameraRotation.Yaw);
    float DeltaPitch = FMath::FindDeltaAngleDegrees(PitchRotation, CameraRotation.Pitch);
    float DeltaRoll = FMath::FindDeltaAngleDegrees(RollRotation, CameraRotation.Roll);
    
    FRotator NewRotation = CameraRotation;
    // Yaw
    if (FMath::Abs(DeltaYaw) > DataPtr_->MaxYawAngle) {
        NewRotation.Yaw = YawRotation + FMath::Clamp(DeltaYaw,-DataPtr_->MaxYawAngle, DataPtr_->MaxYawAngle);
    } else {
        FRotator DesiredRotation = NewRotation;
        DesiredRotation.Yaw = YawRotation + CameraYawOffset_;
        NewRotation = FMath::RInterpTo(NewRotation, DesiredRotation, DeltaTime, DataPtr_->YawLagSpeed); }

    // Pitch
    if (FMath::Abs(DeltaPitch) > DataPtr_->MaxPitchAngle) {
        NewRotation.Pitch = PitchRotation + FMath::Clamp(DeltaPitch, -DataPtr_->MaxPitchAngle, DataPtr_->MaxPitchAngle);
    } else {
        FRotator DesiredRotation = NewRotation;
        DesiredRotation.Pitch = PitchRotation + CameraPitchOffset_;
        NewRotation = FMath::RInterpTo(NewRotation, DesiredRotation, DeltaTime, DataPtr_->PitchLagSpeed); }
    
    // Roll
    if (FMath::Abs(DeltaRoll) > DataPtr_->MaxRollAngle) {
        NewRotation.Roll = RollRotation + FMath::Clamp(DeltaRoll, -DataPtr_->MaxRollAngle, DataPtr_->MaxRollAngle);
    } else {
        FRotator DesiredRotation = NewRotation;
        DesiredRotation.Roll = RollRotation;
        NewRotation = FMath::RInterpTo(NewRotation, DesiredRotation, DeltaTime, DataPtr_->RollLagSpeed); }

    //Apply
    FinalRotation = NewRotation;
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
    float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    float MaxSpeed = PhysicReferencePtr_->GetMaxSpeed();

    if (MaxSpeed <= 0.f)
        return;

    float TargetDistance = PhysicReferencePtr_->GetCurrentSpeedPercentage() * DataPtr_->MaxDistanceToCamera;
    FVector TargetRelativeLocation = FVector(-TargetDistance, 0.f, 0.f);

    FVector NewLocation = FMath::VInterpTo(CameraPtr_->GetRelativeLocation(), TargetRelativeLocation, DeltaTime, DataPtr_->DistanceInterpSpeed);

    CameraPtr_->SetRelativeLocation(NewLocation);

    GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Blue, FString::Printf(TEXT("Speed : %f / Distance : %f/%f"), PhysicReferencePtr_->GetCurrentSpeedPercentage(), TargetDistance, DataPtr_->MaxDistanceToCamera));
}
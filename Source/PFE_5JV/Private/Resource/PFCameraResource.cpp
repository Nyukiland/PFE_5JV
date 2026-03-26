#include "Resource/PFCameraResource.h"

#include "Ability/PFTurnAbility.h"
#include "Camera/CameraComponent.h"
#include "Helpers/PFMathHelper.h"
#include "StateMachine/PFPlayerCharacter.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "Kismet/KismetMathLibrary.h"

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
    SmoothedCameraRotation_ = FRotator(PhysicReferencePtr_->ForwardRootPtr_->GetComponentRotation().Pitch,
        Owner->GetActorRotation().Yaw, 0);
    CurrentTurnRoll_ = 0.f;
    
    CurrentCameraOffset_ = FVector::ZeroVector;
    TargetCameraOffset_ = FVector::ZeroVector;
    CameraFollowLagSpeed_ = DataPtr_->DistanceInterpSpeed * 0.1f;
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CheckValidity())
        return;

    FRotator FinalRootRotation;

    // Camera Root movements
    UpdateCameraRotation(deltaTime, FinalRootRotation);
    UpdateCameraDive(deltaTime);

    CameraRootPtr_->SetWorldRotation(FinalRootRotation);

    // Camera movements
    UpdateCameraDistance(deltaTime);
    
    FVector PlayerCenter = Owner->GetActorLocation();
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(
        CameraPtr_->GetComponentLocation(), 
        PlayerCenter);
    CameraPtr_->SetWorldRotation(LookAtRotation);
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
    if (DiveAbilityPtr_ && DiveAbilityPtr_->IsDiving() && !HasStartedDive_)
    {
        SmoothedPitchDelta_ = 0.f;
        SmoothedRollDelta_  = 0.f;
        HasStartedDive_ = true;
    }
    else if (!DiveAbilityPtr_ || !DiveAbilityPtr_->IsDiving())
    {
        HasStartedDive_ = false;
    }
    
    // ---- YAW ----
    const float BaseYaw = Owner->GetActorRotation().Yaw;
    float InputYaw = TurnAbilityPtr_->InputRight_ - TurnAbilityPtr_->InputLeft_;
    float TargetOvershootYaw = InputYaw * DataPtr_->OvershootYawOffset;
    SmoothedOvershootYaw_ = FMath::FInterpTo(SmoothedOvershootYaw_,TargetOvershootYaw,DeltaTime,DataPtr_->OvershootInterpSpeed);
    float NormalOffset = CameraYawOffset_ + InputYaw;
    NormalOffset = FMath::Clamp(NormalOffset,-DataPtr_->MaxYawAngle,DataPtr_->MaxYawAngle);
    float TargetWorldYaw = BaseYaw + NormalOffset + SmoothedOvershootYaw_;
    FRotator CurrentRotYaw(0.f, SmoothedCameraRotation_.Yaw, 0.f);
    FRotator TargetRotYaw(0.f, TargetWorldYaw, 0.f);
    SmoothedCameraRotation_.Yaw =FMath::RInterpTo(CurrentRotYaw,TargetRotYaw,DeltaTime,DataPtr_->YawLagSpeed).Yaw;
    FinalRotation.Yaw = SmoothedCameraRotation_.Yaw;
    
    // ---- PITCH ----
    const float BasePitch = PhysicReferencePtr_->ForwardRootPtr_->GetComponentRotation().Pitch;
    float TargetDeltaPitch = CameraPitchOffset_;
    TargetDeltaPitch = FMath::Clamp(TargetDeltaPitch,-DataPtr_->MaxPitchAngle,DataPtr_->MaxPitchAngle);
    SmoothedPitchDelta_ = FMath::FInterpTo(SmoothedPitchDelta_,TargetDeltaPitch,DeltaTime,DataPtr_->PitchLagSpeed);
    FinalRotation.Pitch = BasePitch + SmoothedPitchDelta_;
}

void UPFCameraResource::UpdateCameraDive(float DeltaTime)
{
    FRotator BaseLocalRotation(DataPtr_->BasePitchRotation, 0.f, 0.f);
    FRotator DiveLocalRotation(DataPtr_->DivePitchRotation, 0.f, 0.f);
    FVector  BaseLocalLocation(0.f, 0.f, 0.f);
    FVector  DiveLocalLocation(0.f, 0.f, DataPtr_->DiveHeightOffset);

    ElapsedDiveTime += DeltaTime;
    
    // any dive => change camera
    if (!DiveAbilityPtr_->IsDiving())
    {
        if (WasDiving_)
        {
            // Rotation normale
            TargetCameraRotation_ = BaseLocalRotation;
            TargetCameraLocation_ = BaseLocalLocation;
            ShakeTime_ = 0.f;
            WasDiving_ = false;
            HasStartedDiveTransition = false;
        }
    }
    else
    {
        if (!WasDiving_)
        {
            ElapsedDiveTime = 0.f;
            WasDiving_ = true;
        }
        
        if (!HasStartedDiveTransition && ElapsedDiveTime > DataPtr_->DurationBeforeDive)
        {
            // Rotation de dive
            TargetCameraRotation_ = DiveLocalRotation;
            TargetCameraLocation_ = DiveLocalLocation;
            HasStartedDiveTransition = true;
        }

        // Involuntary dive => shake
        if (!DiveAbilityPtr_->AutoDiveComplete())
        {
            ShakeTime_ += DeltaTime;
        
            float NoiseRoll  = FMath::PerlinNoise1D(ShakeTime_ * DataPtr_->ShakeFrequency);
            float NoisePitch = FMath::PerlinNoise1D((ShakeTime_ + 100.f) * DataPtr_->ShakeFrequency);
            float NoiseYaw   = FMath::PerlinNoise1D((ShakeTime_ + 200.f) * DataPtr_->ShakeFrequency);
        
            FRotator ShakeOffset;
            ShakeOffset.Roll  = NoiseRoll  * DataPtr_->RollAmplitude;
            ShakeOffset.Pitch = NoisePitch * DataPtr_->PitchAmplitude;
            ShakeOffset.Yaw   = NoiseYaw   * DataPtr_->YawAmplitude;
            FRotator DiveBase = FRotator(DataPtr_->DivePitchRotation,0,0);
            TargetCameraRotation_ = DiveBase + ShakeOffset;
        }
    }

    FRotator Current = CameraPtr_->GetRelativeRotation();
    FRotator NewRotation = FMath::RInterpTo(Current, TargetCameraRotation_, DeltaTime, DataPtr_->DiveInterpSpeed);
    CameraPtr_->SetRelativeRotation(NewRotation);
    
    FVector CurrentLocation = CameraPtr_->GetRelativeLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetCameraLocation_, DeltaTime, DataPtr_->DiveInterpSpeed);
    CameraPtr_->SetRelativeLocation(NewLocation);
}

void UPFCameraResource::UpdateCameraDistance(float DeltaTime)
{
    const float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    float BaseCameraDistance = UPFMathHelper::RemapClamped(
        CurrentSpeed,
        GlideAbilityDataPtr_->AutoDiveSpeedLimit,
        PhysicReferencePtr_->GetMaxBoostVelocity(),
        DataPtr_->MinDistanceToCamera * 100,
        DataPtr_->MaxDistanceToCamera * 100);
    
    float SpeedRatio = FMath::Clamp(
        CurrentSpeed / PhysicReferencePtr_->GetMaxBoostVelocity(), 
        0.f, 1.f);
    
    float BaseLagSpeed = FMath::Lerp(
        DataPtr_->DistanceInterpSpeed, 
        DataPtr_->DistanceInterpSpeed * 0.3f,
        SpeedRatio);
    
    float CollisionLagSpeed = (CurrentCameraOffset_.SizeSquared() > 0.1f) ? 20.0f : BaseLagSpeed;
    CameraFollowLagSpeed_ = CollisionLagSpeed;
    
    UpdateCameraCollision(DeltaTime);
    
    FVector PlayerCenter = Owner->GetActorLocation();
    
    FVector Backward = -CameraPtr_->GetForwardVector();
    Backward.Normalize();
    
    FVector BaseTargetLocation = PlayerCenter + Backward * BaseCameraDistance;
    BaseTargetLocation.Z += DataPtr_->DiveHeightOffset;
    
    FVector TargetLocation = BaseTargetLocation + CurrentCameraOffset_;
    
    FVector NewLocation = FMath::VInterpTo(
        CameraPtr_->GetComponentLocation(), 
        TargetLocation, 
        DeltaTime, 
        CameraFollowLagSpeed_);
    
    CameraPtr_->SetWorldLocation(NewLocation);
    
    UE_LOG(LogTemp, Warning, TEXT("DEBUG - Loc: %s, Offset: %s, BaseTarget: %s"), 
       *NewLocation.ToString(), 
       *CurrentCameraOffset_.ToString(),
       *BaseTargetLocation.ToString());
}

void UPFCameraResource::UpdateCameraCollision(float DeltaTime)
{
    FVector CameraLocation = CameraPtr_->GetComponentLocation();
    
    const float MaxDetectionDistance = 500.0f;
    const float MinDetectionDistance = 50.0f;
    
    TArray<FVector> TestDirections;
    TestDirections.Add(FVector::UpVector);
    TestDirections.Add(-FVector::UpVector);
    TestDirections.Add(CameraPtr_->GetRightVector());
    TestDirections.Add(-CameraPtr_->GetRightVector());
    
    TargetCameraOffset_ = FVector::ZeroVector;
    float ClosestDistance = FLT_MAX;
    FVector ClosestHitDirection = FVector::ZeroVector;
    
    for (const FVector& Dir : TestDirections)
    {
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(Owner);
        QueryParams.AddIgnoredActor(CameraPtr_->GetOwner());
        
        FVector TraceEnd = CameraLocation + Dir * MaxDetectionDistance;
        
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult, 
            CameraLocation, 
            TraceEnd, 
            ECC_Visibility, 
            QueryParams);
        
        if (bHit)
        {
            if (HitResult.Distance > MaxDetectionDistance || HitResult.Distance < MinDetectionDistance)
            {
                continue;
            }
            
            UE_LOG(LogTemp, Warning, TEXT("COLLISION - Dir: %s, Distance: %f"), *Dir.ToString(), HitResult.Distance);
            
            if (HitResult.Distance < ClosestDistance)
            {
                ClosestDistance = HitResult.Distance;
                ClosestHitDirection = Dir;
            }
        }
    }
    
    if (ClosestDistance < FLT_MAX)
    {
        FVector PushDirection = -ClosestHitDirection;
        PushDirection.Normalize();
        
        float DistanceFactor = 1.0f - (ClosestDistance / MaxDetectionDistance);
        TargetCameraOffset_ = PushDirection * DataPtr_->CollisionPushForce * DistanceFactor;
        
        UE_LOG(LogTemp, Warning, TEXT("CAMERA PUSH - HitDir: %s, PushDir: %s, Offset: %s"), 
               *ClosestHitDirection.ToString(), 
               *PushDirection.ToString(),
               *TargetCameraOffset_.ToString());
    }
    else
    {
        TargetCameraOffset_ = FVector::ZeroVector;
    }
    
    CurrentCameraOffset_ = FMath::VInterpTo(
        CurrentCameraOffset_, 
        TargetCameraOffset_, 
        DeltaTime, 
        15.0f);
}
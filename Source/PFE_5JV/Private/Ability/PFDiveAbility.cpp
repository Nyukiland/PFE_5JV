#include "Ability/PFDiveAbility.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFDiveAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
    Super::ComponentInit_Implementation(ownerObj);

    PhysicResourcePtr_ = Owner->GetStateComponent<UPFPhysicResource>();

    VisualResourcePtr_ = Owner->GetStateComponent<UPFVisualResource>();

    HapticsResource_ = Owner->GetStateComponent<UPFHapticsResource>();
}

void UPFDiveAbility::ComponentDisable_Implementation()
{
    Super::ComponentDisable_Implementation();

    InputLeft_ = 0.0f;
    InputRight_ = 0.0f;
    GetHighestValue();
    TimerAutoDive_ = 0;

    bIsDivingStateGoingUp_ = false;
    SpeedBeforeDive_ = 0.0f;
    GoingUpTimer_ = 10;
    MaxTimeGoingUp_ = 0;

    // Reset Velocity and Smoothing variables
    SmoothedInputLeft_ = 0.0f;
    SmoothedInputRight_ = 0.0f;
    CurrentLeftVelocity_ = 0.0f;
    CurrentRightVelocity_ = 0.0f;
    bWasDivingLastFrame_ = false;
    DiveRollDirection = 0;
}

void UPFDiveAbility::ComponentTick_Implementation(float deltaTime)
{
    if (HighestInput_ != 0)
       ElapsedTime_ += deltaTime;
    else
       ElapsedTime_ = 0;

    Dive(deltaTime);
    AfterDiveGoingUp(deltaTime);
    DiveVisual(deltaTime);
    DiveRoll(deltaTime);
    DiveHaptics();

    if (IsDiving())
    {
       PhysicResourcePtr_->ResetPhysicsTimer();
    }
}

FString UPFDiveAbility::GetInfo_Implementation()
{
    FString text = TEXT("<hb>Dive:</>");
    text += TEXT("\n <b>Auto Dive Value: </>") + FString::Printf(TEXT("%f"), TimerAutoDive_);
    text += TEXT("\n <b>Is Auto Dive Complete: </>") + FString::Printf(
       TEXT("%s"), (AutoDiveComplete() ? TEXT("true") : TEXT("false")));
    text += TEXT("\n");
    text += TEXT("\n <b>Dive Value: </>") + FString::Printf(TEXT("%f"), CurrentMedianValue_);

    return text;
}

void UPFDiveAbility::ReceiveInputLeft(float left)
{
    InputLeft_ = left;
    GetHighestValue();
}

void UPFDiveAbility::ReceiveInputRight(float right)
{
    InputRight_ = right;
    GetHighestValue();
}

void UPFDiveAbility::Dive(float deltaTime)
{
    if (!DataPtr_ || !DataPtr_->DiveAccelerationBasedOnRotationCurvePtr
       || !DataPtr_->DiveAccelerationBasedOnSpeedCurvePtr || !PhysicResourcePtr_)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad Set up on data"));
       return;
    }

    if (!IsDiving())
       return;

    float speedToGive = DataPtr_->ForceToGive *
       DataPtr_->DiveAccelerationBasedOnRotationCurvePtr->GetFloatValue(HighestInput_);

    float velocity0to1 = PhysicResourcePtr_->GetForwardVelocityPercentage();
    speedToGive *= DataPtr_->DiveAccelerationBasedOnSpeedCurvePtr->GetFloatValue(velocity0to1);

    float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
    if (forwardVelo <= DataPtr_->MaxDiveVelocity)
    {
       float toAdd = speedToGive * deltaTime;
       float veloWithAdded = forwardVelo + toAdd;
       float veloDiff = DataPtr_->MaxDiveVelocity - forwardVelo;
       toAdd = DataPtr_->MaxDiveVelocity > veloWithAdded ? toAdd : veloDiff;
       PhysicResourcePtr_->AddForwardVelocity(toAdd, false);
    }
}

void UPFDiveAbility::AfterDiveGoingUp(float deltaTime)
{
    if (!DataPtr_ || !PhysicResourcePtr_
       || !DataPtr_->GoingUpRotationCurve
       || !DataPtr_->GoingUpDurationCurve)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad Set up on data"));
       return;
    }

    if (!DataPtr_->bUseUCurveGoingUp)
       return;

    if (IsDiving() && !bIsDivingStateGoingUp_)
    {
       bIsDivingStateGoingUp_ = true;
       GoingUpTimer_ = 100;
       SpeedBeforeDive_ = PhysicResourcePtr_->GetCurrentVelocity().Length();
    }

    if (!IsDiving() && bIsDivingStateGoingUp_)
    {
       bIsDivingStateGoingUp_ = false;

       float value01 = PhysicResourcePtr_->GetCurrentVelocity().Length() - SpeedBeforeDive_;
       value01 /= DataPtr_->MaxDiveVelocity;
       value01 = FMath::Clamp(value01, 0.0f, 1.0f);
       MaxTimeGoingUp_ = DataPtr_->MaxTimeForGoingUpAfterDive;
       MaxTimeGoingUp_ *= DataPtr_->GoingUpDurationCurve->GetFloatValue(value01);
       GoingUpTimer_ = 0;
    }

    if (bIsDivingStateGoingUp_ || GoingUpTimer_ > MaxTimeGoingUp_)
       return;

    GoingUpTimer_ += deltaTime;

    float timer01 = FMath::Clamp(GoingUpTimer_ / MaxTimeGoingUp_, 0, 1);
    float rotationValue = DataPtr_->MaxUpRotationPitch;
    rotationValue *= DataPtr_->GoingUpRotationCurve->GetFloatValue(timer01);
    PhysicResourcePtr_->SetPitchRotationVisual(rotationValue, -1);
}

void UPFDiveAbility::DiveVisual(float deltaTime)
{
    if (!DataPtr_ || !PhysicResourcePtr_)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
       return;
    }

    float highestValue = FMath::Min(InputLeft_, InputRight_);

    float lerpSpeedToUse = highestValue >= CurrentMedianValue_
                         ? DataPtr_->LerpPitchSpeedGoingUp
                         : DataPtr_->LerpPitchSpeedGoingDown;

    CurrentMedianValue_ = FMath::Lerp(CurrentMedianValue_, highestValue, lerpSpeedToUse * deltaTime);

    if (!IsDiving())
       return;

    float rotAlphaBasedOnCurve = DataPtr_->RotationPitchBasedOnInputCurvePtr->GetFloatValue(CurrentMedianValue_);
    float value = FMath::Lerp(0, DataPtr_->MaxRotationPitch, rotAlphaBasedOnCurve);
    PhysicResourcePtr_->SetPitchRotationVisual(value, -4);
}

void UPFDiveAbility::DiveHaptics()
{
    if (!DataPtr_ || !HapticsResource_ || !DataPtr_->HapticsBasedOnRotation)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
       return;
    }

    FHapticsSettings settings = DataPtr_->HapticsSettings;
    float intensity = DataPtr_->HapticsBasedOnRotation->GetFloatValue(CurrentMedianValue_) * settings.Intensity;
    intensity = FMath::Clamp(intensity, 0.f, 1.f);

    if (!IsDiving())
       intensity = 0;

    HapticsResource_->PlayHaptics(intensity, settings.Duration, "Dive",
                         settings.bAffectsLeftLarge, settings.bAffectsLeftSmall, settings.bAffectsRightLarge,
                         settings.bAffectsRightSmall);
}

void UPFDiveAbility::DiveRoll(float deltaTime)
{
   if (!DataPtr_ || !VisualResourcePtr_)
   {
      UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
      return;
   }

   DiveRollInputRecording(deltaTime);
   DiveRollCheck();

   if (DiveRollDirection == 0)
   {
      TimerWaitToRollDive_ = 0;
      return;
   }

   if (!IsDiving())
      return;

   TimerWaitToRollDive_ += deltaTime;

   if (TimerWaitToRollDive_ < 0.1f)
      return;

   VisualResourcePtr_->AddToRollRotation(DataPtr_->DiveRollRotationForce * DiveRollDirection, -3);
}

bool UPFDiveAbility::IsDiving()
{
    if (!DataInputPtr_)
       return false;

    if (ElapsedTime_ >= DataInputPtr_->DelayBeforeGoingInDive && HighestInput_ != 0)
       return true;

    return false;
}

float UPFDiveAbility::GetDivingValue()
{
    if (!IsDiving())
       return 0;

    return HighestInput_;
}

void UPFDiveAbility::AutoDive(float deltaTime)
{
    if (!DataPtr_ || !PhysicResourcePtr_)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
       return;
    }

    TimerAutoDive_ += deltaTime;

    float value = TimerAutoDive_ / DataPtr_->AutoDiveRotationTime;
    value = FMath::Clamp(value, 0.0f, 1.0f);
    value = FMath::Lerp(0, DataPtr_->AutoDiveDiveRotationPercentage, value);

    ReceiveInputLeft(value);
    ReceiveInputRight(value);
}

bool UPFDiveAbility::AutoDiveComplete() const
{
    if (!DataPtr_)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
       return true;
    }

    return TimerAutoDive_ == 0;
}

bool UPFDiveAbility::AutoDiveRotationComplete() const
{
    if (!DataPtr_)
    {
       UE_LOG(LogTemp, Error, TEXT("[DiveAbility] Bad set up on Data"))
       return true;
    }

    return TimerAutoDive_ > DataPtr_->AutoDiveRotationTime;
}

void UPFDiveAbility::GetHighestValue()
{
    HighestInput_ = FMath::Min(InputLeft_, InputRight_);
}

void UPFDiveAbility::DiveRollInputRecording(float deltaTime)
{
    if (!DataPtr_ || !DataInputPtr_) return;

    bool bIsTryingToDive = HighestInput_ > 0.0f;
    if (bIsTryingToDive && !bWasDivingLastFrame_) 
    {
        SmoothedInputLeft_ = InputLeft_;
        SmoothedInputRight_ = InputRight_;
        RecordedPreviousInputLeft_ = InputLeft_;
        RecordedPreviousInputRight_ = InputRight_;
    }
    bWasDivingLastFrame_ = bIsTryingToDive;

    if (DiveRollDirection != 0) return;

    float alpha = 0.7f; 

    SmoothedInputLeft_ = (InputLeft_ * alpha) + (SmoothedInputLeft_ * (1.0f - alpha));
    SmoothedInputRight_ = (InputRight_ * alpha) + (SmoothedInputRight_ * (1.0f - alpha));

    CurrentLeftVelocity_ = (SmoothedInputLeft_ - RecordedPreviousInputLeft_) / deltaTime;
    CurrentRightVelocity_ = (SmoothedInputRight_ - RecordedPreviousInputRight_) / deltaTime;

    RecordedPreviousInputLeft_ = SmoothedInputLeft_;
    RecordedPreviousInputRight_ = SmoothedInputRight_;
}

void UPFDiveAbility::DiveRollCheck()
{
    if (!DataInputPtr_) return;

    float depthRequired = DataInputPtr_->InputPressedValueThreshold;
    bool bothDeepEnough = (InputLeft_ >= depthRequired) && (InputRight_ >= depthRequired);

    if (!bothDeepEnough)
    {
       DiveRollDirection = 0;
       return;
    }

    if (DiveRollDirection != 0)
       return;

    bool isLeftFlicking = CurrentLeftVelocity_ > DataInputPtr_->InputChangeRequiredDiveRoll;
    bool isRightFlicking = CurrentRightVelocity_ > DataInputPtr_->InputChangeRequiredDiveRoll;

    if (isLeftFlicking && !isRightFlicking)
    {
       DiveRollDirection = -1;
    }
    else if (!isLeftFlicking && isRightFlicking)
    {
       DiveRollDirection = 1;
    }
}
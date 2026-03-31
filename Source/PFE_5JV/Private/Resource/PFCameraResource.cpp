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
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CheckValidity())
        return;
    

    
}

bool UPFCameraResource::CheckValidity() const
{
    if(!CameraPtr_ || !CameraRootPtr_ || !CameraYawPtr_ || !CameraPitchPtr_ || !CameraRollPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("[UPFCameraResource] One or more Gimbal Components are NULL"));
        return false;
    }
    if(!DataPtr_ || !PhysicReferencePtr_ || !TurnAbilityPtr_ || DiveAbilityPtr_)
    {
        UE_LOG(LogTemp, Error, TEXT("Missing Data or Ability references in UPFCameraResource"));
        return false;
    }
    return true;
}

void UPFCameraResource::ManageCameraOffset()
{
    FVector Pos = TrackedTargetPtr_->GetComponentLocation();
    Pos += FVector::BackwardVector * FMath::Lerp()
}
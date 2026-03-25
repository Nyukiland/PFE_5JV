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
}

void UPFCameraResource::ComponentTick_Implementation(float deltaTime)
{
    Super::ComponentTick_Implementation(deltaTime);

    if (!CheckValidity())
        return;
    UE_LOG(LogTemp, Warning, TEXT("DeltaTime=%.4f"), deltaTime);

    FVector FinalRootLocation = CameraRootPtr_->GetComponentLocation();
    FRotator FinalRootRotation = CameraRootPtr_->GetComponentRotation();
    
    UpdateCameraDistance(deltaTime, FinalRootLocation, FinalRootRotation);
    UpdateCameraYaw(deltaTime, FinalRootLocation, FinalRootRotation);
    UpdateCameraPitch(deltaTime, FinalRootLocation, FinalRootRotation);
    UpdateCameraRoll(deltaTime, FinalRootLocation, FinalRootRotation);
    UpdateCameraDive(deltaTime);
    
    CameraRootPtr_->SetWorldLocation(FinalRootLocation);
    CameraRootPtr_->SetWorldRotation(FinalRootRotation);

    UE_LOG(LogTemp, Warning, TEXT("Applied: Location=(%.2f,%.2f,%.2f), Rotation=(%.2f,%.2f,%.2f)"), 
           FinalRootLocation.X, FinalRootLocation.Y, FinalRootLocation.Z,
           FinalRootRotation.Pitch, FinalRootRotation.Yaw, FinalRootRotation.Roll);
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

void UPFCameraResource::UpdateCameraYaw(float DeltaTime, FVector FinalLocation, FRotator& FinalRotation)
{
    FVector PlayerLocation = Owner->GetActorLocation();
    FVector DirectionToPlayer = PlayerLocation - FinalLocation;
    DirectionToPlayer.Normalize();
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(FinalLocation, PlayerLocation);
    float TargetYaw = TargetRotation.Yaw;
    float NewYaw = FMath::FInterpTo(FinalRotation.Yaw, TargetYaw, DeltaTime, DataPtr_->YawLagSpeed);
    FinalRotation.Yaw = NewYaw;
}

void UPFCameraResource::UpdateCameraPitch(float DeltaTime, FVector FinalLocation, FRotator& FinalRotation)
{
    FVector PlayerLocation = Owner->GetActorLocation();
    FVector DirectionToPlayer = PlayerLocation - FinalLocation;
    DirectionToPlayer.Normalize();
    UE_LOG(LogTemp, Warning, TEXT("Camera Z: %.2f, Player Z: %.2f, Diff: %.2f"), 
           FinalLocation.Z, PlayerLocation.Z, PlayerLocation.Z - FinalLocation.Z);
    
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(FinalLocation, PlayerLocation);

    float TargetPitch = TargetRotation.Pitch;
    UE_LOG(LogTemp, Warning, TEXT("TargetPitch=%.2f, CurrentPitch=%.2f, Diff=%.2f"), 
               TargetPitch, FinalRotation.Pitch, TargetPitch - FinalRotation.Pitch);
    
    float NewPitch = FMath::FInterpTo(FinalRotation.Pitch, TargetPitch, DeltaTime, DataPtr_->PitchLagSpeed);

    FinalRotation.Pitch = NewPitch;
}

void UPFCameraResource::UpdateCameraRoll(float DeltaTime, FVector FinalLocation, FRotator& FinalRotation)
{
    FinalRotation.Roll = 0.f;
}

void UPFCameraResource::UpdateCameraDive(float DeltaTime)
{
    
}

void UPFCameraResource::UpdateCameraDistance(float DeltaTime, FVector& FinalLocation, FRotator FinalRotation)
{
    const float CurrentSpeed = PhysicReferencePtr_->GetCurrentVelocity().Length();
    const float CameraDistance = UPFMathHelper::RemapClamped(
        CurrentSpeed,
        GlideAbilityDataPtr_->AutoDiveSpeedLimit,
        PhysicReferencePtr_->GetMaxBoostVelocity(),
        DataPtr_->MinDistanceToCamera * 100,
        DataPtr_->MaxDistanceToCamera * 100);
    
    FVector PlayerCenter = Owner->GetActorLocation();
    FVector Backward = -Owner.Get()->GetActorForwardVector();
    Backward.Normalize();
    FVector TargetLocation = PlayerCenter + Backward * CameraDistance;
    float OldZ = FinalLocation.Z;
    TargetLocation.Z += DataPtr_->DiveOffset.Z;
    FVector NewLocation = FMath::VInterpTo(FinalLocation, TargetLocation, DeltaTime, DataPtr_->DistanceInterpSpeed);
    
    FinalLocation = NewLocation;

    UE_LOG(LogTemp, Warning, TEXT("Z Interp: Old=%.2f, New=%.2f, Target=%.2f, Move=%.2f"), 
           OldZ, NewLocation.Z, TargetLocation.Z, NewLocation.Z - OldZ);

}
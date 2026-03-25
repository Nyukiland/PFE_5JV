#pragma once

#include "CoreMinimal.h"
#include "PFPhysicResource.h"
#include "Ability/PFDiveAbility.h"
#include "Ability/Data/PFGlideAbilityData.h"
#include "Data/PFCameraResourceData.h"
#include "State/PFAutoDiveState.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCameraResource.generated.h"

class UPFTurnAbility;
class UCameraComponent;
class USceneComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class PFE_5JV_API UPFCameraResource : public UPFResource
{
    GENERATED_BODY()

public:

    virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
    virtual void ComponentTick_Implementation(float deltaTime) override;

    /* ===== Data Dependencies ===== */
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Data")
    TObjectPtr<UPFCameraResourceData> DataPtr_;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Data")
    TObjectPtr<UPFGlideAbilityData> GlideAbilityDataPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFPhysicResource> PhysicReferencePtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFTurnAbility> TurnAbilityPtr_;

    // The specific object the camera tracks and copies rotation from
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<USceneComponent> TrackedTargetPtr_;

    /* ===== Gimbal Rig Components ===== */

    // Handles World Position (Distance / Dive Offset)
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraRootPtr_;

    // Child of Root. Handles World Yaw
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraYawPtr_;

    // Child of Yaw. Handles Local Pitch
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraPitchPtr_;

    // Child of Pitch. Handles Local Roll
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraRollPtr_;

    // Child of Roll. The actual view
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<UCameraComponent> CameraPtr_;

private:

    bool CheckValidity() const;
    
    void UpdateCameraDistance(float DeltaTime);
    void UpdateCameraDive(float DeltaTime);
    void UpdateCameraYaw(float DeltaTime);
    void UpdateCameraPitch(float DeltaTime);
    void UpdateCameraRoll(float DeltaTime);
    void ApplyProceduralEffects(float DeltaTime);

    // Tracked Base Transforms (Prevents shake/overshoot from breaking interp)
    FVector BaseLocation_;
    float BaseYaw_ = 0.f;
    float BasePitch_ = 0.f;
    float BaseRoll_ = 0.f;

    // Movement tracking
    float PreviousPlayerYaw_ = 0.f;
    
    // Procedural Offsets
    float SmoothedOvershootYaw_ = 0.f;
    float CurrentShakePitch_ = 0.f;
    float CurrentShakeYaw_ = 0.f;

    // Dive State
    bool IsCurrentlyDiving_ = false;
    float ElapsedDiveTime_ = 0.f;
};
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

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFCameraResource : public UPFResource
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Data")
    TObjectPtr<UPFCameraResourceData> DataPtr_;

    // C++ Ref
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFPhysicResource> PhysicReferencePtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
    TObjectPtr<UPFTurnAbility> TurnAbilityPtr_;

    // Comp in bp link
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> TrackedTargetPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraRootPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraYawPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraPitchPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<USceneComponent> CameraRollPtr_;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|Components")
    TObjectPtr<UCameraComponent> CameraPtr_;
    
public:
    virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
    virtual void ComponentTick_Implementation(float deltaTime) override;
    
private:
    bool CheckValidity() const;

    void ManageCameraOffset();
};
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
class USpringArmComponent;

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class PFE_5JV_API UPFCameraResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPFCameraResource();

	void ComponentInit_Implementation(APFPlayerCharacter* ownerObj);
	virtual void ComponentTick_Implementation(float deltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPFCameraResourceData> DataPtr_;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPFGlideAbilityData> GlideAbilityDataPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UCameraComponent> CameraPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<USceneComponent> CameraRootPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPrimitiveComponent> VisualResourcePtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPFPhysicResource> PhysicReferencePtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPFDiveAbility> DiveAbilityPtr_;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
	TObjectPtr<UPFTurnAbility> TurnAbilityPtr_;

private:

	bool CheckValidity() const;
	
	void UpdateCameraRotation(float DeltaTime, FRotator& FinalRotation);
	void UpdateCameraShake(float DeltaTime);
	void UpdateCameraDistance(float DeltaTime);

	// Base rotation
	float CameraYawOffset_   = 0.f;
	float CameraPitchOffset_ = 0.f;
	float CameraRollOffset_  = 0.f;
	float LastActorYaw_   = 0.f;
	float LastActorPitch_ = 0.f;
	float LastActorRoll_  = 0.f;
	float SmoothedOvershootYaw_  = 0.f;

	// Shake
	float ShakeTime_ = 0.f;

	// Roll
	float PreviousYaw_ = 0.f;
	float BaseCameraRoll_ = 0.f;
	float CurrentTurnRoll_ = 0.f;
	FRotator FinalBaseRotation_;
	FRotator SmoothedCameraRotation_;

	// Dive
	bool HasStartedDive_ = false;
	float SmoothedPitchDelta_ = 0.0f;
	float SmoothedRollDelta_ = 0.0f;
};
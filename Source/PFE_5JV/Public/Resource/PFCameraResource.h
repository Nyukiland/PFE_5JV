#pragma once

#include "CoreMinimal.h"
#include "Data/PFCameraResourceData.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFCameraResource.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PFE_5JV_API UPFCameraResource : public UPFResource
{
	GENERATED_BODY()

public:
	UPFCameraResource();
	
protected:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<USceneComponent> CameraRootPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<USpringArmComponent> SpringArmPtr_;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	TObjectPtr<UCameraComponent> CameraPtr_;
	
	UFUNCTION(BlueprintCallable)
	void UpdateRotation(float deltaTime);
	UFUNCTION(BlueprintCallable)
	void UpdatePosition(float deltaTime);
	UFUNCTION(blueprintCallable)
	void UpdateZoom(float deltaTime);

	FVector ComputeTargetLocation() const;

	UPROPERTY(EditAnywhere, blueprintReadWrite)
	TObjectPtr<UPFCameraResourceData> CameraResourceData_;

private:
	float DiveTheTimer = 0.f;
};

/*
 * Structure recommandee
 *
 * BirdPawn
 * └── Root
 *  ├── Mesh
 * 	└── CameraRoot (SceneComponent)
 * 		└── SpringArm
 * 			└── Camera
 */

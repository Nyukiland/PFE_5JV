#pragma once

#include "CoreMinimal.h"
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
	USceneComponent* CameraRoot;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera|References")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float Distance = 420.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float Height = 120.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float LookAhead = 80.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float PositionInterpSpeed = 3.5f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float RotationInterpSpeed = 6.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float RollFactor = 0.25f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float NormalFOV = 90.f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float FOVInterpSpeed = 2.5f;

	UFUNCTION(BlueprintCallable)
	void UpdateRotation(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void UpdatePosition(float DeltaTime);

	FVector ComputeTargetLocation() const;
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
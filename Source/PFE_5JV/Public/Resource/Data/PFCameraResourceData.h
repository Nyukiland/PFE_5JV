#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
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

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float MaxPitchWhenRotating = 5.0f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	float MaxYawWhenRotating = 20.0f;

	UPROPERTY(EditAnywhere, Category="Camera|Settings")
	bool IsDiving = false;
};

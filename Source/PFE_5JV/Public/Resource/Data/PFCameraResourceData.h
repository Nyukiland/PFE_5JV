#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()
public:
	
	/* ===== Rotation ===== */

	UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	float MaxYawAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	float MaxPitchAngle = 30.f;
	
	// UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	// float MaxRollAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	float YawLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	float PitchLagSpeed = 20.f;

	// UPROPERTY(EditAnywhere, Category = "Base Rotation", meta=(ToolTip=""))
	// float RollLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float LookAtInterpSpeed = 5.0f;
	
	/* ===== Overshoot ===== */
	
	UPROPERTY(EditAnywhere, Category = "Overshoot", meta=(ToolTip=""))
	float OvershootYawOffset = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Overshoot", meta=(ToolTip=""))
	float OvershootInterpSpeed = 5.0f;

	/* ===== Shake ===== */

	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float ShakeFrequency = 20.f;

	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float RollAmplitude = 25.f;
	
	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float PitchAmplitude = 10.f;

	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float YawAmplitude = 8.f;

	/* ===== Distance ===== */

	UPROPERTY(EditAnywhere, Category = "Distance", meta = (ToolTip = ""))
	float MinDistanceToCamera = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Distance", meta = (ToolTip = ""))
	float MaxDistanceToCamera = 20.f;

	UPROPERTY(EditAnywhere, Category = "Distance", meta = (ToolTip = ""))
	float DistanceInterpSpeed = 5.f;

	/* ===== Dive ===== */
	
	UPROPERTY(EditAnywhere, Category = "Dive", meta = (ToolTip = ""))
	float BasePitchRotation = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Dive", meta = (ToolTip = ""))
	float DivePitchRotation = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Dive", meta = (ToolTip = ""))
	float DiveHeightOffset = 0.f;

	UPROPERTY(EditAnywhere, Category = "Dive", meta=(ToolTip=""))
	float DiveInterpSpeed = 5.0f;
};
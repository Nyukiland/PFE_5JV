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

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float MaxYawAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float MaxPitchAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float MaxRollAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float YawLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float PitchLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float RollLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float OvershootYawOffset = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation", meta=(ToolTip=""))
	float CameraYawInputOffset = 5.0f;

	/* ===== Shake ===== */

	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float ShakeFrequency = 20.f;

	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float RollAmplitude = 25.f;
	
	UPROPERTY(EditAnywhere, Category = "Shake", meta=(ToolTip=""))
	float PitchAmplitude = 10.f;
	
	float YawAmplitude = 8.f;

	/* ===== Roll ===== */

	UPROPERTY(EditAnywhere, Category = "Roll", meta=(ToolTip="Inclinaison maximum"))
	float MaxTurnRoll = 30.f;

	UPROPERTY(EditAnywhere, Category = "Roll", meta=(ToolTip="Vitesse de retour"))
	float TurnRollSpeed = 0.08f;

	UPROPERTY(EditAnywhere, Category = "Roll", meta=(ToolTip="Conversion de la vitesse vers le roll"))
	float TurnSensitivity = 6.f;

	/* ===== Distance ===== */

	UPROPERTY(EditAnywhere, Category = "Distance", meta = (ToolTip = ""))
	float MaxDistanceToCamera = 20.f;

	UPROPERTY(EditAnywhere, Category = "Distance", meta = (ToolTip = ""))
	float DistanceInterpSpeed = 5.f;
};
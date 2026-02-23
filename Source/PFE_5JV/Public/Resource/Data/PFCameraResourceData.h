#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	
	/* ===== Rotation Lag Speed ===== */

	UPROPERTY(EditAnywhere, Category = "Camera Lag", meta=(ToolTip=""))
	float YawLagSpeed = 2.f;

	UPROPERTY(EditAnywhere, Category = "Camera Lag", meta=(ToolTip=""))
	float PitchLagSpeed = 2.f;

	UPROPERTY(EditAnywhere, Category = "Camera Lag", meta=(ToolTip=""))
	float RollLagSpeed = 2.f;

	/* ===== Max Rotation Angle ===== */
	
	UPROPERTY(EditAnywhere, Category = "Max Angle", meta=(ToolTip=""))
	float MaxYawAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Max Angle", meta=(ToolTip=""))
	float MaxPitchAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Max Angle", meta=(ToolTip=""))
	float MaxRollAngle = 30.f;

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
};
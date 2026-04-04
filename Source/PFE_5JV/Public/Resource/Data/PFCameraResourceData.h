#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()
public:
	
	// Position Stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float BaseZOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToBaseLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float DiveZOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToDiveLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	TObjectPtr<UCurveFloat> DiveOffsetCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float WingBeatZOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToWingBeatLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	TObjectPtr<UCurveFloat> WingBeatOffsetCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float TurnYOffset = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetTurnLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetTurnToBaseLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	TObjectPtr<UCurveFloat> TurnOffsetCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	TObjectPtr<UCurveFloat> TurnOffsetSpeedCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float BaseDistanceWithPlayer = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float MaxDistanceWithPlayer = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float DistanceLerpSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	TObjectPtr<UCurveFloat> DistanceCurve;
	
	// Rotation Stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Rotation")
	float MaxYawAngle = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Rotation")
	float MaxPitchAngle = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Rotation")
	float YawLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Rotation")
	float PitchLagSpeed = 20.f;

	// True Cam Rot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float BaseCamRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float CamRotLerpSpeedToBase;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float DiveCamRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float CamRotLerpSpeedToDive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	TObjectPtr<UCurveFloat> DiveCamRotCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float WingBeatCamRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	float CamRotLerpSpeedToWingBeat;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "True Cam Rotation")
	TObjectPtr<UCurveFloat> WingBeatCamRotCurve;
};
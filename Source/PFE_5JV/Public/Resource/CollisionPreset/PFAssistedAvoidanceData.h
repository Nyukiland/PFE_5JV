#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFAssistedAvoidanceData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFAssistedAvoidanceData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Ray
	UPROPERTY(EditAnywhere, Category = "Ray")
	float ForwardShieldDistance = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Ray")
	float IntentSphereDistance = 2500.f;

	UPROPERTY(EditAnywhere, Category = "Ray")
	float IntentSphereRadius = 150.f;

	// Turn
	UPROPERTY(EditAnywhere, Category = "Turn")
	float MaxExpectedYawVelocity = 90.f; 

	UPROPERTY(EditAnywhere, Category = "Turn")
	float MaxExpectedPitchRotation = 45.f;

	// Curve
	UPROPERTY(EditAnywhere, Category = "Curves",
		meta = (ToolTip = "Turn value over the ray direction"))
	TObjectPtr<UCurveFloat> RayDirectionBlendCurve;

	UPROPERTY(EditAnywhere, Category = "Curves",
		meta = (ToolTip = "Distance value over the force"))
	TObjectPtr<UCurveFloat> AvoidanceForceCurve;

	UPROPERTY(EditAnywhere, Category = "Curves",
		meta = (ToolTip = "Turn value over redirection force, o being the normal and 1 being the normal"))
	TObjectPtr<UCurveFloat> TurnPropulsionSplitCurve;

	// Force
	UPROPERTY(EditAnywhere, Category = "Forces")
	float BasePitchAssist = 15.f;

	UPROPERTY(EditAnywhere, Category = "Forces")
	float BaseYawAssist = 15.f;

	UPROPERTY(EditAnywhere, Category = "Forces")
	float TurnPropelForce = 800.f; 

	// Debug
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bShowDebug = false;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFHardAvoidanceData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFHardAvoidanceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Detection",
		meta = (ClampMin = 0, ClampMax = 90, ToolTip = "The angle spread of the 9 predictive raycasts."))
	float ConeAngle = 30;
	
		// -------------------------------------------------------------------
	// Assist

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ToolTip = "Distance at which the bird starts gently steering away from obstacles."))
	float AssistDistance = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ToolTip = "Distance at which the bird starts gently steering away from obstacles on sides."))
	float AssistDistanceSides = 1750;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ToolTip = "Distance at which the bird starts gently steering away from obstacles on sides."))
	float AssistDistanceDiagonal = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ToolTip = "How much velocity is applied to slide the bird away."))
	float AssistForce = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ToolTip = "How fast the bird's nose visually pitches/yaws away during an assist."))
	float AssistTurnSpeed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist",
		meta = (ClampMin = 0))
	float SmoothingTurn = 5;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	float SlowForceAssist = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	TObjectPtr<UCurveFloat> AssistForceCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	TObjectPtr<UCurveFloat> AssistTurnCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	float MinSpeedMultiplier = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	float MaxSpeedMultiplier = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	TObjectPtr<UCurveFloat> AssistSpeedMultiplierCurve;

	// -------------------------------------------------------------------
	// Avoid

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Avoid",
		meta = (ToolTip = "Distance at which the system takes over and forcefully stops a crash."))
	float AvoidDistance = 800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Avoid",
		meta = (ToolTip = "The strength of the emergency push away from the wall."))
	float AvoidForce = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Avoid",
		meta = (ToolTip = "How fast the bird physically yaws away during an emergency avoid."))
	float AvoidForceRot = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Assist")
	float SlowForceAvoid = 0.95f;
	
};

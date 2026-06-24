#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCollisionResourceData.generated.h"

class UPFAfterCollisionState;

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCollisionResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Detection

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Detection",
		meta = (ClampMin = 0, ClampMax = 90, ToolTip = "The angle spread of the 9 predictive raycasts."))
	float ConeAngle = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Detection",
		meta = (ToolTip = "Radius of the sphere used for predictive sweeping."))
	float PreshotSphereSize = 35;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Detection",
		meta = (ToolTip = "Distance of the immediate right/left/up/down rays to detect hugging walls."))
	float FlankDetectionDist = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Detection",
		meta = (ToolTip = "Distance of the immediate right/left/up/down rays to detect hugging walls.",
			ClampMin = 0, ClampMax = 1))
	float FlankPredictionDist = 0.5f;

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

	// -------------------------------------------------------------------
	// Hard Impact

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ClampMax = 1, ClampMin = 0, ToolTip =
			"Dot product threshold between velocity and wall normal to be considered a Hard Collision."))
	float ThresholdHorizontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ToolTip = "Speed multiplier applied when scraping a wall on the side."))
	float SlowPercentageAfterSideCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ToolTip = "Speed multiplier applied when crashing head-on into a wall."))
	float SlowPercentageAfterFrontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ToolTip = "State to transition to if a hard collision occurs."))
	TSubclassOf<UPFAfterCollisionState> AfterCollisionState;

	// -------------------------------------------------------------------
	// Rollback

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Rollback",
		meta = (ToolTip = "If true, hitting a wall head-on rewinds time slightly instead of just stopping."))
	bool bUseRollBackOnFrontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Rollback",
		meta = (ToolTip = "How many seconds of flight history the system keeps in memory."))
	float DurationOfRemember;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Rollback",
		meta = (ToolTip = "How many seconds back in time the player is teleported upon crash."))
	float DurationOfRewind;

	// -------------------------------------------------------------------
	// Debug

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Debug",
		meta = (ToolTip = "Draws the prediction cone and repulsion vectors in the editor."))
	bool bShowPredictiveDebug = true;
};

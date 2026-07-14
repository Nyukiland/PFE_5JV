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
	float ConeAngle = 35.0f;
    UPROPERTY(EditAnywhere, Category = "Ray") 
    float PreshotSphereSize = 50.0f;
    UPROPERTY(EditAnywhere, Category = "Ray") 
    float CriticalBrakeDistance = 800.0f;

	// Distances
    UPROPERTY(EditAnywhere, Category = "Distances") 
    float AssistDistance = 2000.f;
    UPROPERTY(EditAnywhere, Category = "Distances") 
    float AssistDistanceSides = 1500.f;
    UPROPERTY(EditAnywhere, Category = "Distances") 
    float AssistDistanceDiagonal = 1750.f;
    UPROPERTY(EditAnywhere, Category = "Distances") 
    float AvoidDistance = 800.f;

	// Intent
    UPROPERTY(EditAnywhere, Category = "Intent") 
    float MaxAllowedIntentAngle = 80.0f; 
    UPROPERTY(EditAnywhere, Category = "Intent") 
    TObjectPtr<UCurveFloat> DistanceAllowanceCurve;
    UPROPERTY(EditAnywhere, Category = "Intent") 
    TObjectPtr<UCurveFloat> DirectionAllowanceCurve;
    UPROPERTY(EditAnywhere, Category = "Intent") 
    float IntentAcceptanceThreshold = 0.5f;

	// Speed
    UPROPERTY(EditAnywhere, Category = "Speed") 
    float MinSpeedMultiplier = 1.0f;
    UPROPERTY(EditAnywhere, Category = "Speed") 
    float MaxSpeedMultiplier = 2.5f;
    UPROPERTY(EditAnywhere, Category = "Speed") 
    TObjectPtr<UCurveFloat> AssistSpeedMultiplierCurve;

	// Forces
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float AvoidForce = 500.f;
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float AvoidForceRot = 10.f;
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float AssistForce = 250.f;
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float AssistTurnSpeed = 5.f;
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float SlowForceAvoid = 1000.f;
    UPROPERTY(EditAnywhere, Category = "Forces") 
    float SlowForceAssist = 500.f;

	// Curves
    UPROPERTY(EditAnywhere, Category = "Curves") 
    TObjectPtr<UCurveFloat> AssistTurnCurve;
    UPROPERTY(EditAnywhere, Category = "Curves") 
    TObjectPtr<UCurveFloat> AssistForceCurve;
    UPROPERTY(EditAnywhere, Category = "Curves") 
    float SmoothingTurn = 5.0f;

	// Debug
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
};

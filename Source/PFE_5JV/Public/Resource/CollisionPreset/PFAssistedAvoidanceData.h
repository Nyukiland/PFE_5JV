#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFAssistedAvoidanceData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFAssistedAvoidanceData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Detection
	UPROPERTY(EditAnywhere, Category = "Detection")
	 float IntentSphereRadius = 200.f;
	UPROPERTY(EditAnywhere, Category = "Detection")
	 float IntentSphereDistance = 3000.f;
	UPROPERTY(EditAnywhere, Category = "Detection")
	 float WhiskerSpread = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Detection")
	 float SmoothingSpeed = 5.0f;

	// Forces
	UPROPERTY(EditAnywhere, Category = "Forces")
	 float BasePitchAssist = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Forces")
	 float BaseYawAssist = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Forces")
	 float TurnPropelForce = 800.f;
};

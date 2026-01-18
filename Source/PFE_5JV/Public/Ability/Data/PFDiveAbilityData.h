#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFDiveAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFDiveAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta = (ToolTip = "The acceleration based on how much the player is looking down, 0 = front, 1 = down"))
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnRotationCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta = (ToolTip = "The acceleration based on the player current speed, 0 = slow, 1 = max speed"))
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnSpeedCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta = (ToolTip = "The base force given to the player (will be multiplied by the curve)"))
	float ForceToGive;
};

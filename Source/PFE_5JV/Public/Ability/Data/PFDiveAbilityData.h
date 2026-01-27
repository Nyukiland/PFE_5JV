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
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnRotationCurvePtr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta = (ToolTip = "The acceleration based on the player current speed, 0 = slow, 1 = max speed"))
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnSpeedCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive",
		meta = (ToolTip = "The base force given to the player (will be multiplied by the curve)"))
	float ForceToGive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dive/PitchRotation",
	meta = (ToolTip = "Max rotation down when diving"))
	float MaxRotationPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dive/PitchRotation",
		meta = (ToolTip = "Speed at which the bird rotate up"))
	float LerpPitchSpeedGoingUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dive/PitchRotation",
		meta = (ToolTip = "Speed at which the bird rotate down"))
	float LerpPitchSpeedGoingDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "How long the bird will take to rotate, make sure it's under half the duration"))
	float AutoDiveRotationTime = 0.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "The dive rotation", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AutoDiveDiveRotationPercentage = 1.0f;
};

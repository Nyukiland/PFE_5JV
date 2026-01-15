#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFWingVisualAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFWingVisualAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation")
	float MaxRotationPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation")
	float LerpPitchSpeedGoingUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation")
	float LerpPitchSpeedGoingDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/WingRotation")
	float MaxWingRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/WingRotation")
	float LerpWingRotation;
};

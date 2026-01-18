#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFWingVisualAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFWingVisualAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation",
		meta = (ToolTip = "Max rotation down when diving"))
	float MaxRotationPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation",
		meta = (ToolTip = "Speed at which the bird rotate up"))
	float LerpPitchSpeedGoingUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/PitchRotation",
		meta = (ToolTip = "Speed at which the bird rotate down"))
	float LerpPitchSpeedGoingDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/WingRotation",
		meta = (ToolTip = "Max rotation when the bird is turnin"))
	float MaxWingRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual/WingRotation",
		meta = (ToolTip = "Speed at which the bird rotate when turning"))
	float LerpWingRotation;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFWingVisualAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFWingVisualAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	float MaxRotationPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	float LerpPitchSpeedGoingUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	float LerpPitchSpeedGoingDown;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	float BaseWingRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WingVisual")
	float MaxWingRotation;
};

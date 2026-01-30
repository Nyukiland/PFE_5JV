#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFTurnAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFTurnAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The rotation force given to the player"))
	float RotationForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The amount of rotation based on the current input"))
	TObjectPtr<UCurveFloat> RotationForceBasedOnInputPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The amount of rotation based on the current velocity"))
	TObjectPtr<UCurveFloat> RotationForceBasedOnVelocityPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn", meta = (ClampMin = 0, ClampMax = 1),
		meta = (ToolTip = "The minimum difference between the two input to consider them as different"))
	float ToleranceRotBetweenInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn/WingRotation",
	meta = (ToolTip = "Max rotation when the bird is turnin"))
	float MaxWingRotation;
};

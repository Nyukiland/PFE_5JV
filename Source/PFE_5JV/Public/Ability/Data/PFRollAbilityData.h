#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFRollAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFRollAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "Max time between each input in order to roll"))
	float RollRepetitionTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "When the double tap is valid how long it will wait before calling the roll \n used to prevent input collision"))
	float WaitBeforeCallingWhenValid = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "Max time between each input in order to roll"))
	float ThresholdInput = 0.9f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "How long will the player stay in roll"))
	float RollDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "The force apply to the side of the bird in roll"))
	float RollForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "The force applied based on the time elapsed"))
	TObjectPtr<UCurveFloat> RollForceOverTimePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll",
		meta = (ToolTip = "Number of visual rotation the bird will do"))
	int RotationCount;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCollisionResourceData.generated.h"

class UPFCollisionPreset;

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCollisionResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPFCollisionPreset> DefaultCollisionPreset;
	
	// Detection

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
	// Debug

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Debug",
		meta = (ToolTip = "Draws the prediction cone and repulsion vectors in the editor."))
	bool bShowPredictiveDebug = true;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFStrictCollisionData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFStrictCollisionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,
		meta = (ClampMax = 1, ClampMin = 0, ToolTip =
			"Dot product threshold between velocity and wall normal to be considered a Hard Collision."))
	float ThresholdHorizontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ToolTip = "Speed multiplier applied when scraping a wall on the side."))
	float SlowPercentageAfterSideCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact",
		meta = (ToolTip = "Speed multiplier applied when crashing head-on into a wall."))
	float SlowPercentageAfterFrontalCollision;
};

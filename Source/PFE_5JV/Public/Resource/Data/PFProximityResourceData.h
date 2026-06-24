#pragma once
#include "PFProximityResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFProximityResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|ForwardSphere")
	float ForwardSphereSize = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|ForwardSphere")
	float ForwardSphereDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|ClosestSphere")
	float BaseClosestSphereSize = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|ClosestSphere")
	float SmallestClosestSphereSize = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|ClosestSphere")
	float ClosestSphereDetectionStep = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|Below")
	float BelowRayDistance = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|Debug")
	bool bisDrawDebugForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|Debug")
	bool bisDrawDebugClosest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity|Debug")
	bool bisDrawDebugBelow;
};

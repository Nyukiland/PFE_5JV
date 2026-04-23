#pragma once

#include "CoreMinimal.h"
#include "StateMachine/StateComponent/PFResource.h"
#include "PFProximityResource.generated.h"

UCLASS()
class PFE_5JV_API UPFProximityResource : public UPFResource
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CheckCollisions();
	void CheckCollisions_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushSphereDistance = 4000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	FVector2D BrushSizesMinMax = FVector2D(50, 150);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	float BrushForwardOffset = 2500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	TArray<FHitResult> ValidHitResults;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Proximity")
	TArray<float> ValidBrushSizes;
};
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFDiveAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFDiveAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnRotationCurve;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UCurveFloat> DiveAccelerationBasedOnSpeedCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	float ForceToGive;
};

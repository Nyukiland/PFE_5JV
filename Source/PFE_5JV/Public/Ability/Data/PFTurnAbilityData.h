#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFTurnAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFTurnAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	float RotationForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	TObjectPtr<UCurveFloat> RotationForceBasedOnInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn")
	TObjectPtr<UCurveFloat> RotationForceBasedOnVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn", meta = (ClampMin = 0, ClampMax = 1))
	float ToleranceRotBetweenInput;
};

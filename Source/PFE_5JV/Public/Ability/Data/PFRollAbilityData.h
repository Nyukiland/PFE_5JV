#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFRollAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFRollAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	float RollDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	float RollForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	TObjectPtr<UCurveFloat> RollForceOverTimePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roll")
	int RotationCount;
};

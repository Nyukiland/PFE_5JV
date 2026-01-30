#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFVisualResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFVisualResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	float LerpWingRotation;
};

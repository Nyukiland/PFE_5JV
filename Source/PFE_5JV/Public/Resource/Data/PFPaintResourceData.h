#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFPaintResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFPaintResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	FVector2D BrushSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	float BrushMaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	FVector2D BrushSizeLandscape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Paint")
	float BrushMaxDistanceLandscape;
};

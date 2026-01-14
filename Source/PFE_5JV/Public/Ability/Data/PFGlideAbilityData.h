#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFGlideAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFGlideAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/Movement")
	float BaseForwardMovement;
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFPhysicResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFPhysicResourceData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxSpeed = 10;
	
};

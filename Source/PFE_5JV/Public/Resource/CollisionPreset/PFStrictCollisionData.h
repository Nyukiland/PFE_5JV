#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFStrictCollisionData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFStrictCollisionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision|Hard Impact")
	float SlowOnCollision = -400;
};

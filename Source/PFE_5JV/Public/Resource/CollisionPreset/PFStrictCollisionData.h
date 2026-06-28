#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFStrictCollisionData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PFE_5JV_API UPFStrictCollisionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionOffset = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float SlowOnCollision = -400;
};

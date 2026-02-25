#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCollisionResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCollisionResourceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "When colliding face on should we roll back or dir"))
	bool bUseRollBackOnFrontalCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "How long will the player roll back when colliding with the wall"))
	float DurationOfRemember;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "The amount of speed reduced when the player collides with the wall"))
	float SlowAfterFrontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "The amount of speed reduced when the player collides with the wall on the side"))
	float SlowAfterSideCollision;
};

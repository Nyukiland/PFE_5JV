#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCollisionResourceData.generated.h"

class UPFAfterCollisionState;

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
		meta = (ToolTip = "How long will the player roll back when colliding with the wall"))
	float DurationOfRewind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "The amount of speed reduced when the player collides with the wall"))
	float SlowPercentageAfterFrontalCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ToolTip = "The amount of speed reduced when the player collides with the wall on the side"))
	float SlowPercentageAfterSideCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision",
		meta = (ClampMax = 1, ClampMin = 0))
	float ThresholdHorizontalCollision;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PreshotSphereSize = 35;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float SweepAnticipation = 1.8f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	TSubclassOf<UPFAfterCollisionState> AfterCollisionState;
};

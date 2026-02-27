#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFTurnAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFTurnAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The rotation force given to the player", ClampMin = 0))
	float RotationForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The amount of rotation based on the current input"))
	TObjectPtr<UCurveFloat> RotationForceBasedOnInputPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn",
		meta = (ToolTip = "The amount of rotation based on the current velocity"))
	TObjectPtr<UCurveFloat> RotationForceBasedOnVelocityPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn/SlowDown",
		meta = (ToolTip = "The slowDown force given to the player when turning", ClampMin = 0))
	float SlowForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn/SlowDown",
		meta = (ToolTip = "The amount of slow used based on the current input"))
	TObjectPtr<UCurveFloat> SlowForceBasedOnInputPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn/SlowDown",
		meta = (ToolTip = "The amount of slow used based on the current velocity"))
	TObjectPtr<UCurveFloat> SlowForceBasedOnVelocityPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn/SlowDown",
		meta = (ToolTip = "The time required to reach the full slowForce", ClampMin = 0))
	float TimeToGoFullSlowForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Turn/SlowDown",
		meta = (ToolTip = "The curve of the force based on the timer for slowForce"))
	TObjectPtr<UCurveFloat> SlowForceInProgressCurvePtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turn/WingRotation",
	meta = (ToolTip = "Max rotation when the bird is turnin"))
	float MaxWingRotation;
};

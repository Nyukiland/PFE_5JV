#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFCameraResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFCameraResourceData : public UDataAsset
{
	GENERATED_BODY()
public:
	
	// Position Stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float BaseZOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float DiveZOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float TurnYOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetTurnLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToBaseLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToDiveLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	float OffsetGoToWingBeatLerpSpeed = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float BaseDistanceWithPlayer = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float MaxDistanceWithPlayer = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Distance")
	float DistanceLerpSpeed = 2;
	
	// Rotation Stuff
	UPROPERTY(EditAnywhere, Category = "Base Rotation")
	float MaxYawAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Base Rotation")
	float MaxPitchAngle = 30.f;
	
	UPROPERTY(EditAnywhere, Category = "Base Rotation")
	float YawLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Base Rotation")
	float PitchLagSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = "Rotation")
	float LookAtInterpSpeed = 5.0f;
};
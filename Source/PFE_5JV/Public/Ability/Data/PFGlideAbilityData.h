#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PFGlideAbilityData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFGlideAbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/Movement",
		meta = (ToolTip = "The movement speed, always applied"))
	float BaseForwardMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/JoystickMovement",
		meta = (ToolTip = "Should the joystick be used"))
	bool bUseJoystickMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/JoystickMovement",
		meta = (ToolTip = "How much is the bird rotated when looking up or down"))
	float PitchForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/JoystickMovement",
		meta = (ToolTip = "Should the joystick be inverted"))
	bool bUseInvertedPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/JoystickMovement",
		meta = (ToolTip = "Force applied to the player when moving left right"))
	float RightForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "The speed at which the bird auto dive"))
	float AutoDiveSpeedLimit = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "How long will the auto dive last"))
	float AutoDiveDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "How long the bird will take to rotate, make sure it's under half the duration"))
	float AutoDiveRotationTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "The speed the bird will gain by the end of the auto dive"))
	float AutoDiveSpeedGain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Glide/AutoDive",
		meta = (ToolTip = "Speed gained over time"))
	TObjectPtr<UCurveFloat> AutoDiveSpeedCurve;
};

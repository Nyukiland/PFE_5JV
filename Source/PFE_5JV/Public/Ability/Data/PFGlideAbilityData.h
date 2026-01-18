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
};

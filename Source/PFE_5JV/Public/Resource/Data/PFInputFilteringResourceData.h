#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PFInputFilteringResourceData.generated.h"

UCLASS(Blueprintable, BlueprintType)
class PFE_5JV_API UPFInputFilteringResourceData : public UDataTable
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input/InputConfig",
		meta = (ClampMin = 0, ClampMax = 1,
			ToolTip = "The trigger must be pressed to this point to be considered as fully pressed"))
	float InputThresholdPressed = 0.95f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input/InputConfig",
		meta = (ClampMin = 0, ClampMax = 1,
		ToolTip = "The trigger must be release to this point to be considered as having no input"))
	float InputThresholdReleased = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input/Dive",
		meta = (ClampMin = 0, ClampMax = 1,
			ToolTip = "The trigger must be release to this point to be considered as having no input"))
	float DiveRequiredPressedTime = 0.05f;
};

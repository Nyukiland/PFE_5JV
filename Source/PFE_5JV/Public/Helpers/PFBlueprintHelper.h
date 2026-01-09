#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PFBlueprintHelper.generated.h"

UENUM()
enum class EKeyboardOutputPin : uint8
{
	FrenchKeyboard,
	EnglishKeyboard,
	Unknown
};

UCLASS()
class PFE_5JV_API UPFBlueprintHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void CheckCurrentLayout(EKeyboardOutputPin& outputPins);

	UFUNCTION(BlueprintCallable, Category = "Helper")
	static float& SmoothValueByRef(UPARAM(ref) float& curValue, const float valueToReach, const float smoothSpeed, const float deltaTime);
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PFBlueprintHelper.generated.h"

enum class ETriggerEvent : uint8;

UENUM()
enum class EKeyboardOutputPin : uint8
{
	FrenchKeyboard,
	EnglishKeyboard,
	Unknown
};

UENUM(BlueprintType, Blueprintable)
enum class EPlayerInput : uint8
{
	LeftTrigger,
	RightTrigger,
	WingBeat,
	Move,
	NONE,
};

UENUM(BlueprintType)
enum class EExecPinsResult : uint8
{
	Passed,
	Blocked
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

	UFUNCTION(BlueprintCallable, Category = "Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void FilterAllInputRaw(FName givenInputName, EPlayerInput& outputPins);

	UFUNCTION(BlueprintCallable, Category = "Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void FilterAllInputWithTrigger(FName givenInputName, ETriggerEvent triggerToFilter, ETriggerEvent givenTrigger,  EPlayerInput& outputPins);

	UFUNCTION(BlueprintCallable, Category = "Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void FilterSpecificInputRaw(EPlayerInput inputToFilter, FName givenInputName, EExecPinsResult& outputPins);
	
	UFUNCTION(BlueprintCallable, Category = "Helper", meta = (ExpandEnumAsExecs = "outputPins"))
	static void FilterSpecificInputWithTrigger(EPlayerInput inputToFilter, ETriggerEvent triggerToFilter, FName givenInputName, ETriggerEvent givenTrigger, EExecPinsResult& outputPins);

	UFUNCTION(BlueprintCallable, Category = "Helper")
	static FName GetInputNameFromEnum(EPlayerInput Input);
	
	UFUNCTION(BlueprintCallable, Category = "Helper")
	static EPlayerInput  GetEnumFromInputName(FName inputName);
};

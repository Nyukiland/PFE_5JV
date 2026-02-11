#include "Helpers/PFBlueprintHelper.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>

void UPFBlueprintHelper::CheckCurrentLayout(EKeyboardOutputPin& outputPins)
{
	// Get the current keyboard layout
	HKL keyboardLayout = GetKeyboardLayout(0); // Get layout for the active thread
	TCHAR buffer[KL_NAMELENGTH];

	if (GetKeyboardLayoutName(buffer))
	{
		FString layoutName(buffer);

		if (layoutName == "00000409") // US QWERTY
		{
			outputPins = EKeyboardOutputPin::EnglishKeyboard;
		}
		else if (layoutName == "0000040C") // French AZERTY
		{
			outputPins = EKeyboardOutputPin::FrenchKeyboard;
		}
		else
		{
			outputPins = EKeyboardOutputPin::Unknown;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve keyboard layout name"));
	}
}

float& UPFBlueprintHelper::SmoothValueByRef(float& curValue, const float valueToReach, const float smoothSpeed,
                                            const float deltaTime)
{
	float t = smoothSpeed * deltaTime;
	t = FMath::Clamp(t, 0, 1);

	curValue = FMath::Lerp(curValue, valueToReach, t);
	return curValue;
}

void UPFBlueprintHelper::FilterAllInputRaw(FName givenInputName, EPlayerInput& outputPins)
{
	outputPins = GetEnumFromInputName(givenInputName);
}

void UPFBlueprintHelper::FilterAllInputWithTrigger(FName givenInputName, ETriggerEvent triggerToFilter,
                                                   ETriggerEvent givenTrigger, EPlayerInput& outputPins)
{
	if (givenTrigger != triggerToFilter)
	{
		outputPins = EPlayerInput::NONE;
		return;
	}

	outputPins = GetEnumFromInputName(givenInputName);
}

void UPFBlueprintHelper::FilterSpecificInputRaw(EPlayerInput inputToFilter, FName givenInputName, EExecPinsResult& outputPins)
{
	if (givenInputName != GetInputNameFromEnum(inputToFilter))
	{
		outputPins = EExecPinsResult::Blocked;
		return;
	}

	outputPins = EExecPinsResult::Passed;
}

void UPFBlueprintHelper::FilterSpecificInputWithTrigger(EPlayerInput inputToFilter, ETriggerEvent triggerToFilter,
                                                        FName givenInputName, ETriggerEvent givenTrigger,
                                                        EExecPinsResult& outputPins)
{
	if (givenTrigger != triggerToFilter)
	{
		outputPins = EExecPinsResult::Blocked;
		return;
	}

	if (givenInputName != GetInputNameFromEnum(inputToFilter))
	{
		outputPins = EExecPinsResult::Blocked;
		return;
	}

	outputPins = EExecPinsResult::Passed;
}

FName UPFBlueprintHelper::GetInputNameFromEnum(EPlayerInput Input)
{
	switch (Input)
	{
	case EPlayerInput::LeftTrigger:
		return "IA_LeftTrigger";

	case EPlayerInput::RightTrigger:
		return "IA_RightTrigger";

	case EPlayerInput::WingBeat:
		return "IA_WingBeat";

	case EPlayerInput::Move:
		return "IA_Move";

	default:
		return NAME_None;
	}
}

EPlayerInput UPFBlueprintHelper::GetEnumFromInputName(FName inputName)
{
	static const TMap<FName, EPlayerInput> InputMap =
	{
		{ "IA_LeftTrigger",  EPlayerInput::LeftTrigger },
		{ "IA_RightTrigger", EPlayerInput::RightTrigger },
		{ "IA_WingBeat",     EPlayerInput::WingBeat },
		{ "IA_Move",         EPlayerInput::Move }
	};

	if (const EPlayerInput* Found = InputMap.Find(inputName))
	{
		return *Found;
	}

	return EPlayerInput::NONE;
}

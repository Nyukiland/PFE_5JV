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

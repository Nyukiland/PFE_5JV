// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PFTriggerSimultaneousTap.h"

#include "EnhancedPlayerInput.h"

void UPFTriggerSimultaneousTap::ResetSimultaneousTap()
{
	// Reset so it only fires once per combo
	LastTapTimeA = -1.0;
	LastTapTimeB = -1.0;
		
	if (bIsActivatedOnce == true)
	{
		bIsActivatedOnce = false;
	}

	if(bIsSecondInputTooLate == true)
	{
		bIsSecondInputTooLate = false;
	}
}

ETriggerState UPFTriggerSimultaneousTap::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                    FInputActionValue ModifiedValue, const float DeltaTime)
{
	if (!PlayerInput || !ActionAPtr || !ActionBPtr)
	{
		return ETriggerState::None;
	}
	
	// if the Action A timer is already runing, continue to run by adding deltatime to the previous value :
	if(LastTapTimeA >= 0.f)
	{
		LastTapTimeA += DeltaTime;
	}

	// if the Action B timer is already running, continue to run by adding deltatime to the previous value :
	if(LastTapTimeB >= 0.f)
	{
		LastTapTimeB += DeltaTime;
	}
	
	// Get action instances
	const FInputActionInstance* InstanceAPtr = PlayerInput->FindActionInstanceData(ActionAPtr);
	const FInputActionInstance* InstanceBPtr = PlayerInput->FindActionInstanceData(ActionBPtr);
		
	if (!InstanceAPtr || !InstanceBPtr)
	{
		return ETriggerState::None;
	}

	// if the considered input buttons to activate the effect are partially or totally released, we reset :
	if(InstanceAPtr->GetValue().Get<float>() != 1.0f && InstanceBPtr->GetValue().Get<float>() != 1.0f)
	{
		ResetSimultaneousTap();
	}

	// if the second input is too late, stop code :
	if(bIsSecondInputTooLate == true)
	{
		return ETriggerState::None;
	}

	// if button for Action A is down, we launch the Action A timer if not already done :
	if (InstanceAPtr->GetValue().Get<float>() == 1.0f)
	{
		if(LastTapTimeA == -1.0)
		{
			LastTapTimeA = 0;
		}
	}

	// if button for Action B is down, we launch the Action B timer if not already done :
	if (InstanceBPtr->GetValue().Get<float>() == 1.0f)
	{
		if(LastTapTimeB == -1.0)
		{
			LastTapTimeB = 0;
		}
	}

	// if the effect has not be already triggered :
	if(bIsActivatedOnce == false)
	{
		// if the two buttons are currently fully pressed (so the timers are running) :
		if (LastTapTimeA > 0 && LastTapTimeB > 0)
		{
			const double TimeDiff = FMath::Abs(LastTapTimeA - LastTapTimeB);
			
			// if the delay between the two buttons pressed are lower than the tolerance, trigger the effect :  
			if (TimeDiff <= TapTolerance)
			{
				GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Cyan, FString::Printf(TEXT("Tolerance acceptable : %f <= %f"),  TimeDiff, TapTolerance));
				bIsActivatedOnce = true;
				return ETriggerState::Triggered;
			}

			// if the delay between the two buttons pressed are upper than the tolerance, do nothing : 
			if (TimeDiff > TapTolerance)
			{
				bIsSecondInputTooLate = true;
				return ETriggerState::None;
			}
		}
	}
	
	return ETriggerState::None;
}

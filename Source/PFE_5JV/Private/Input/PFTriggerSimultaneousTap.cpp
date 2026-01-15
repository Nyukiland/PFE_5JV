// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PFTriggerSimultaneousTap.h"

#include "EnhancedPlayerInput.h"

void UPFTriggerSimultaneousTap::ResetSimultaneousTap()
{
	// Reset so it only fires once per combo
	LastTapTimeA = -1.0;
	LastTapTimeB = -1.0;
	// UE_LOG(LogTemp, Warning, TEXT("Reset les timers"));
		
	if (bisActivatedOnce == true)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Reset bisActivatedOnce"));
		bisActivatedOnce = false;
	}

	if(bisSecondInputTooLate == true)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Reset bisSecondInputTooLate"));
		bisSecondInputTooLate = false;
	}
}

ETriggerState UPFTriggerSimultaneousTap::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput,
                                                                    FInputActionValue ModifiedValue, float DeltaTime)
{
	if (!PlayerInput || !ActionA || !ActionB)
	{
		return ETriggerState::None;
	}
	
	// FString result = ActionA.GetName();
	// UE_LOG(LogTemp, Warning, TEXT("Action A : %s"), *result);
	// FString result2 = ActionB.GetName();
	// UE_LOG(LogTemp, Warning, TEXT("Action B : %s"), *result2);

	// if the Action A timer is already runing, continue to run by adding deltatime to the previous value :
	if(LastTapTimeA >= 0.f)
	{
		LastTapTimeA += DeltaTime;
		// UE_LOG(LogTemp, Warning, TEXT("LastTapTimeA : %f"), LastTapTimeA);
	}

	// if the Action B timer is already runing, continue to run by adding deltatime to the previous value :
	if(LastTapTimeB >= 0.f)
	{
		LastTapTimeB += DeltaTime;
		// UE_LOG(LogTemp, Warning, TEXT("LastTapTimeB : %f"), LastTapTimeB);
	}
	
	// Get action instances
	const FInputActionInstance* InstanceA = PlayerInput->FindActionInstanceData(ActionA);
	const FInputActionInstance* InstanceB = PlayerInput->FindActionInstanceData(ActionB);
		
	if (!InstanceA || !InstanceB)
	{
		return ETriggerState::None;
	}

	// if the considered input buttons to activate the effect are partially or totally realeased, we reset :
	if(InstanceA->GetValue().Get<float>() != 1.0f && InstanceB->GetValue().Get<float>() != 1.0f)
	{
		ResetSimultaneousTap();
	}

	// if the second input is too late, stop code :
	if(bisSecondInputTooLate == true)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Too Late > stop code"));
		return ETriggerState::None;
	}

	// if button for Action A is down, we launch the Action A timer if not already done :
	if (InstanceA->GetValue().Get<float>() == 1.0f)
	{
		// FString result = InstanceA->GetValue().ToString();
		// UE_LOG(LogTemp, Warning, TEXT("Instance A : %s"), *result);
		if(LastTapTimeA == -1.0)
		{
			LastTapTimeA = 0;
		}
	}

	// if button for Action B is down, we launch the Action B timer if not already done :
	if (InstanceB->GetValue().Get<float>() == 1.0f)
	{
		// FString result2 = InstanceB->GetValue().ToString();
		// UE_LOG(LogTemp, Warning, TEXT("Instance B : %s"), *result2);
		if(LastTapTimeB == -1.0)
		{
			LastTapTimeB = 0;
		}
	}

	// if the effect has not be already triggered :
	if(bisActivatedOnce == false)
	{
		// if the two buttons are currently fully pressed (so the timers are running) :
		if (LastTapTimeA > 0 && LastTapTimeB > 0)
		{
			// UE_LOG(LogTemp, Warning, TEXT("2 temps au-dessus de 0"));
			const double TimeDiff = FMath::Abs(LastTapTimeA - LastTapTimeB);
			// UE_LOG(LogTemp, Warning, TEXT("TimeDiff : %f"), TimeDiff);

			// if the delay between the two buttons pressed are lower than the tolerance, trigger the effect :  
			if (TimeDiff <= TapTolerance)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Déclenche le combo (ActivateOnce > True)"));
				bisActivatedOnce = true;
				return ETriggerState::Triggered;
			}

			// if the delay between the two buttons pressed are upper than the tolerance, do nothing : 
			if (TimeDiff > TapTolerance)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Déclenche PAS le combo"));
				bisSecondInputTooLate = true;
				return ETriggerState::None;
			}
		}
	}
	
	// UE_LOG(LogTemp, Warning, TEXT("Marche pas"));
	return ETriggerState::None;
}

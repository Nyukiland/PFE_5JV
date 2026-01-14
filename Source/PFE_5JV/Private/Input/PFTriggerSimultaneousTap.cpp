// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/PFTriggerSimultaneousTap.h"

#include "EnhancedPlayerInput.h"

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
	if(LastTapTimeA >= 0.f)
	{
		LastTapTimeA += PlayerInput->GetWorld()->DeltaTimeSeconds;
	}

	if(LastTapTimeB >= 0.f)
	{
		LastTapTimeB += PlayerInput->GetWorld()->DeltaTimeSeconds;
	}
	

	// Get action instances
	const FInputActionInstance* InstanceA = PlayerInput->FindActionInstanceData(ActionA);
	const FInputActionInstance* InstanceB = PlayerInput->FindActionInstanceData(ActionB);

	if (bisActivatedOnce == true)
	{
		if(InstanceA->GetValue().Get<float>() != 1.0f && InstanceB->GetValue().Get<float>() != 1.0f)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Reset le bool"));
			bisActivatedOnce = false;
		}
	}
	
	if (!InstanceA || !InstanceB)
	{
		return ETriggerState::None;
	}

	// Detect taps (Triggered THIS frame)
	if (InstanceA->GetValue().Get<float>() == 1.0f)
	{
		FString result = InstanceA->GetValue().ToString();
		// UE_LOG(LogTemp, Warning, TEXT("Instance A : %s"), *result);
		if(LastTapTimeA == -1.0)
		{
			LastTapTimeA = 0;
		}
	}

	// Detect taps (Triggered THIS frame)
	if (InstanceB->GetValue().Get<float>() == 1.0f)
	{
		FString result2 = InstanceB->GetValue().ToString();
		// UE_LOG(LogTemp, Warning, TEXT("Instance B : %s"), *result2);
		if(LastTapTimeB == -1.0)
		{
			LastTapTimeB = 0;
		}
	}

	if(LastTapTimeA > TapTolerance || LastTapTimeB > TapTolerance) {
		// Reset so it only fires once per combo
		LastTapTimeA = -1.0;
		LastTapTimeB = -1.0;
	}


	// Both tapped at least once
	if (LastTapTimeA > 0 && LastTapTimeB > 0)
	{
		// UE_LOG(LogTemp, Warning, TEXT("2 temps au-dessus de 0"));
		// UE_LOG(LogTemp, Warning, TEXT("LastTapTimeA : %f"), LastTapTimeA);
		// UE_LOG(LogTemp, Warning, TEXT("LastTapTimeB : %f"), LastTapTimeB);
		const double TimeDiff = FMath::Abs(LastTapTimeA - LastTapTimeB);

		// Reset so it only fires once per combo
		LastTapTimeA = -1.0;
		LastTapTimeB = -1.0;

		if(!bisActivatedOnce)
		{
			if (TimeDiff <= TapTolerance)
        	{
        		// UE_LOG(LogTemp, Warning, TEXT("Déclenche le combo"));
        		// UE_LOG(LogTemp, Warning, TEXT("TimeDiff : %f"), TimeDiff);
        		bisActivatedOnce = true;
        		return ETriggerState::Triggered;
        	}
		}
		
		if (TimeDiff > TapTolerance)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Déclenche PAS le combo"));
			// UE_LOG(LogTemp, Warning, TEXT("TimeDiff : %f"), TimeDiff);
			return ETriggerState::None;
		}
	}
	// UE_LOG(LogTemp, Warning, TEXT("Marche pas"));
	return ETriggerState::None;
}
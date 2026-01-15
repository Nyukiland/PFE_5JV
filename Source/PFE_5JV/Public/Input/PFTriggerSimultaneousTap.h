// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "PFTriggerSimultaneousTap.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Simultaneous Tap (Custom trigger)"))
class PFE_5JV_API UPFTriggerSimultaneousTap : public UInputTrigger
{
	GENERATED_BODY()
	
public:

	/** First input action to monitor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simultaneous Tap")
	TObjectPtr<const UInputAction> ActionA;

	/** Second input action to monitor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simultaneous Tap")
	TObjectPtr<const UInputAction> ActionB;

	/** Max allowed time difference between taps (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simultaneous Tap", meta = (ClampMin = "0.0"))
	float TapTolerance = 0.15f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Simultaneous Tap")
	bool bisActivatedOnce = false;
	bool bisSecondInputTooLate = false;

protected:
	void ResetSimultaneousTap();
	virtual ETriggerState UpdateState_Implementation(
		const UEnhancedPlayerInput* PlayerInput,
		FInputActionValue ModifiedValue,
		float DeltaTime) override;

private:
	mutable double LastTapTimeA = -1.0;
	mutable double LastTapTimeB = -1.0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "PFTriggerSimultaneousTap.generated.h"

class UPFInputFilteringData;
/**
 * 
 */
UCLASS(meta = (DisplayName = "Simultaneous Tap (Custom trigger)"))
class PFE_5JV_API UPFTriggerSimultaneousTap : public UInputTrigger
{
	GENERATED_BODY()
	
public:
	UPFTriggerSimultaneousTap();
	
	/** First input action to monitor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simultaneous Tap")
	TObjectPtr<const UInputAction> ActionAPtr;

	/** Second input action to monitor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Simultaneous Tap")
	TObjectPtr<const UInputAction> ActionBPtr;

	/** Max allowed time difference between taps (seconds) : to add in Input/DA_InputFilteringData in editor */
	float TapTolerance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Simultaneous Tap")
	bool bIsActivatedOnce = false;
	bool bIsSecondInputTooLate = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFInputFilteringData> DataPtr_;
	
	void ResetSimultaneousTap();
	virtual ETriggerState UpdateState_Implementation(
		const UEnhancedPlayerInput* PlayerInput,
		FInputActionValue ModifiedValue,
		float DeltaTime) override;

private:
	mutable double LastTapTimeA = -1.0;
	mutable double LastTapTimeB = -1.0;
};

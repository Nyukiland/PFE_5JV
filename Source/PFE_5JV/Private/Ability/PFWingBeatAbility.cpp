#include "Ability/PFWingBeatAbility.h"

#include "MathUtil.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/PFVisualResource.h"
#include "StateMachine/PFPlayerCharacter.h"

class UPFVisualResource;

FString UPFWingBeatAbility::GetInfo_Implementation()
{
	FString text = TEXT("<hb>WingBeat:</>");
	text += TEXT("\n <b>CurrentHeight: </>") + FString::Printf(TEXT("%f"), CurrentHeight_);
	text += TEXT("\n <b>HeightAtWingBeatBeginning: </>") + FString::Printf(TEXT("%f"), HeightAtWingBeatBeginning_);
	text += TEXT("\n <b>MaxHeightGain: </>") + FString::Printf(TEXT("%f"), MaxHeightGain_);
	text += TEXT("\n <b>EffectiveHeightGainAfter3S: </>") + FString::Printf(TEXT("%f"), EffectiveHeightGainAfter3S_);
	text += TEXT("\n <b>SuperBeatWingTimer_: </>") + FString::Printf(TEXT("%f"), SuperBeatWingTimer_);
	text += TEXT("\n <b>bIsSuperBeatWingPossible_: </>") + FString::Printf(TEXT("%d"), bIsSuperBeatWingPossible_);

	return text;
}

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	// registration of inputs to compare with inputs a certain amount of time later
	InputRegistrationTimer_ -= deltaTime;
	if(InputRegistrationTimer_ <= 0.f)
	{
		PreviousInputRightRegistered_ = PreviousInputRight_;
		PreviousInputLeftRegistered_ = PreviousInputLeft_;
		InputRightRegistered_ = InputRight_;
		InputLeftRegistered_ = InputLeft_;
		InputRegistrationTimer_ = DataPtr_->DelayBetweenInputRegistrations;
	}

	// if the timer is initialized, we increase it :
	if(SuperBeatWingTimer_ >= 0.f) SuperBeatWingTimer_ += deltaTime;

	// if the timer is just launched but not in SuperWingBeatTimer yet : 
	if(SuperBeatWingTimer_ >= 0.f && SuperBeatWingTimer_ < SuperWingBeatMinTiming_)
	{
		bIsSuperBeatWingPossible_ = false;
		if(SuperWingBeatDebugSpherePtr_->GetVisibleFlag() == true)
		{
			SuperWingBeatDebugSpherePtr_->SetVisibility(false);
		}
	}
	// if the timer is in timing range for trigger the SuperWingBeat :
	else if(SuperBeatWingTimer_ >= SuperWingBeatMinTiming_ && SuperBeatWingTimer_ <= SuperWingBeatMaxTiming_)
	{
		if(bIsSuperBeatWingPossible_ == false)
		{
			OnSuperWingBeatPossible.Broadcast();
			bIsSuperBeatWingPossible_ = true;
			SuperWingBeatDebugSpherePtr_->SetVisibility(true);
		}
	}
	// if the right timing is over :  
	else if(SuperBeatWingTimer_ > SuperWingBeatMaxTiming_)
	{
		if(SuperBeatWingTimer_ != -1.f)
		{
			OnSuperWingBeatPossibilityOver.Broadcast();
			SuperBeatWingTimer_ = -1.f; // Deactivate timer
			bIsSuperBeatWingPossible_ = false;
			SuperWingBeatDebugSpherePtr_->SetVisibility(false);
		}
	}
	
	if(Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight_)
	{
		MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	}
	CurrentHeight_ = Owner->ForwardRootPtr->GetComponentLocation().Z;

	if(TimeEffectiveHeightCalculus_ > 0.f)
	{
		TimeEffectiveHeightCalculus_ -= deltaTime;
		TimeEffectiveHeightCalculus_= FMath::Clamp(TimeEffectiveHeightCalculus_, 0.0f, 3.0f);
		if(TimeEffectiveHeightCalculus_ == 0.f)
		{
			EffectiveHeightGainAfter3S_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
			TimeEffectiveHeightCalculus_ = -1.f;
		}
	}	
}

void UPFWingBeatAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);
	
	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();
	
	if (!DataPtr_ || !DataPtr_->DelayBetweenInputRegistrations)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'DelayBetweenInputRegistrations'"));
		return;
	}
	InputRegistrationTimer_ = DataPtr_->DelayBetweenInputRegistrations;

	if (!DataPtr_ || !DataPtr_->SuperWingBeatTiming)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatTiming'"));
		return;
	}
	SuperWingBeatMinTiming_ = DataPtr_->SuperWingBeatTiming;
	
	if (!DataPtr_ || !DataPtr_->SuperWingBeatTolerance)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatTolerance'"));
		return;
	}
	SuperWingBeatMaxTiming_ = DataPtr_->SuperWingBeatTiming + DataPtr_->SuperWingBeatTolerance;
}

void UPFWingBeatAbility::ComponentEnable_Implementation()
{
	Super::ComponentEnable_Implementation();

	SuperBeatWingTimer_ = -1.f;
	InputRegistrationTimer_ = DataPtr_->DelayBetweenInputRegistrations;
}

void UPFWingBeatAbility::ReceiveInputLeft(float left)
{
	PreviousInputLeft_ = InputLeft_;
	InputLeft_ = left;
}

void UPFWingBeatAbility::ReceiveInputRight(float right)
{
	PreviousInputRight_ = InputRight_;
	InputRight_ = right;
}

void UPFWingBeatAbility::WingBeat(float deltaTime)
{
	if (!DataPtr_ || !DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data WingBeatAccelerationBasedOnAverageInputValueCurve"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->WingBeatHeightGainedBasedOnForceDurationCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data WingBeatHeightGainedBasedOnForceDurationCurve"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatForwardMultiplier)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatMultiplier'"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatHeightMultiplier)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatHeightMultiplier'"));
		return;
	}

	// Debug Data :
	HeightAtWingBeatBeginning_ = Owner->ForwardRootPtr->GetComponentLocation().Z;;
	TimeEffectiveHeightCalculus_ = 3.f;
	if(bIsSuperBeatWingPossible_)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SuperBeatWing Triggered")));
		OnSuperWingBeatSucceeded.Broadcast();
	} else
	{
		OnWingBeatCalled.Broadcast();
	} 
		
	// Give the average input value to calculate the wingbeat power : 
	GetAverageInputValue();
	
	float HeightToGive = DataPtr_->VelocityToGiveInHeight *
		DataPtr_->WingBeatHeightGainedBasedOnForceDurationCurve->GetFloatValue(DataPtr_->VelocityToGiveInHeightDuration);
	if(bIsSuperBeatWingPossible_ == true) HeightToGive *= DataPtr_->SuperWingBeatHeightMultiplier;
	PhysicResourcePtr_->AddVelocity(HeightToGive * FVector::UpVector, true, false, DataPtr_->VelocityToGiveInHeightDuration);

	// Debug data :
	MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	
	float VelocityToGive = DataPtr_->VelocityToGiveInForward *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);

	float maxVelocity = DataPtr_->MaxWingBeatForwardVelocity;
	if(bIsSuperBeatWingPossible_ == true)
	{
		maxVelocity = DataPtr_->MaxSuperWingBeatVelocity;
		VelocityToGive *= DataPtr_->SuperWingBeatForwardMultiplier;
		
	}
	float ForwardVelocity = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	if(ForwardVelocity <= maxVelocity)
	{
		float MaxVelocityCanGive = maxVelocity - ForwardVelocity;
		float toAdd = VelocityToGive < MaxVelocityCanGive ? VelocityToGive : MaxVelocityCanGive;
		PhysicResourcePtr_->AddForwardVelocity(toAdd, false);
	}

	PhysicResourcePtr_->ResetPhysicsTimer();
	
	// Trigger timer for SuperBeatWing :
	SuperBeatWingTimer_ = 0.f;
	OnSuperWingBeatTimerLaunch.Broadcast();
}

void UPFWingBeatAbility::GetAverageInputValue()
{
	float RightInputDiff = FMathf::Abs(PreviousInputRightRegistered_ - 1);
	float LeftInputDiff = FMathf::Abs(PreviousInputLeftRegistered_ - 1);
	
	AverageInputValue_ = (RightInputDiff + LeftInputDiff)/2;
}

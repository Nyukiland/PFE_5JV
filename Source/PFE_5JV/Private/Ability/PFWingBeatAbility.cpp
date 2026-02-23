#include "Ability/PFWingBeatAbility.h"

#include "MathUtil.h"
#include "Ability/Data/PFDiveAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

FString UPFWingBeatAbility::GetInfo_Implementation()
{
	FString text = TEXT("<hb>WingBeat:</>");
	text += TEXT("\n <b>CurrentHeight: </>") + FString::Printf(TEXT("%f"), CurrentHeight);
	text += TEXT("\n <b>HeightAtWingBeatBeginning: </>") + FString::Printf(TEXT("%f"), HeightAtWingBeatBeginning);
	text += TEXT("\n <b>MaxHeightGain: </>") + FString::Printf(TEXT("%f"), MaxHeightGain);
	text += TEXT("\n <b>EffectiveHeightGainAfter3S: </>") + FString::Printf(TEXT("%f"), EffectiveHeightGainAfter3S);
	return text;
}

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	// registration of inputs to compare with inputs a certain amount of time later
	TimeUntilNextInputRegistration -= deltaTime;
	if(TimeUntilNextInputRegistration <= 0.f)
	{
		PreviousInputRightRegistered_ = PreviousInputRight_;
		PreviousInputLeftRegistered_ = PreviousInputLeft_;
		InputRightRegistered_ = InputRight_;
		InputLeftRegistered_ = InputLeft_;
		TimeUntilNextInputRegistration = DataPtr_->DelayBetweenInputRegistrations;
	}

	TimeLeftToTriggerSuperBeatWing -= deltaTime;
	
	if(Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight)
	{
		MaxHeightGain = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning;
	}
	CurrentHeight = Owner->ForwardRootPtr->GetComponentLocation().Z;

	if(TimeEffectiveHeightCalculus > 0.f)
	{
		TimeEffectiveHeightCalculus -= deltaTime;
		TimeEffectiveHeightCalculus= FMath::Clamp(TimeEffectiveHeightCalculus, 0.0f, 3.0f);
		if(TimeEffectiveHeightCalculus == 0.f)
		{
			EffectiveHeightGainAfter3S = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning;
			TimeEffectiveHeightCalculus = -1.f;
		}
	}	
}

void UPFWingBeatAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);
	
	PhysicResource_ = CastChecked<UPFPhysicResource>
	(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
	
	if (!DataPtr_ || !DataPtr_->DelayBetweenInputRegistrations)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'DelayBetweenInputRegistrations'"));
		return;
	}
	TimeUntilNextInputRegistration = DataPtr_->DelayBetweenInputRegistrations;

	if (!DataPtr_ || !DataPtr_->SuperWingBeatTiming)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatTiming'"));
		return;
	}
	TimeLeftToTriggerSuperBeatWing = DataPtr_->SuperWingBeatTiming;
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
	// Debug Data :
	HeightAtWingBeatBeginning = Owner->ForwardRootPtr->GetComponentLocation().Z;;
	TimeEffectiveHeightCalculus = 3.f;
	
	if (!DataPtr_ || !DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data WingBeatAccelerationBasedOnAverageInputValueCurve"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatVelocityMultiplier)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatMultiplier'"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatHeightMultiplier)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatHeightMultiplier'"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatTiming)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatTiming'"));
		return;
	}

	const bool bIsSuperBeatWingActivated = (TimeLeftToTriggerSuperBeatWing > 0.f);
	if(bIsSuperBeatWingActivated == true) UE_LOG(LogTemp, Error, TEXT("SuperBeatWingActivated"));
	
	GetAverageInputValue();
	
	float HeightToGive = DataPtr_->ForceToGiveInHeight *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	if(bIsSuperBeatWingActivated == true) HeightToGive *= DataPtr_->SuperWingBeatHeightMultiplier;
	PhysicResource_->AddForce(HeightToGive * FVector::UpVector, true, false, DataPtr_->ForceToGiveInHeightDuration);

	// Debug
	MaxHeightGain = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning;
	
	float SpeedToGive = DataPtr_->ForceToGiveInVelocity *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	if(bIsSuperBeatWingActivated == true) SpeedToGive *= DataPtr_->SuperWingBeatVelocityMultiplier;

	PhysicResource_->AddForwardForce(SpeedToGive, false);

	PhysicResource_->ResetPhysicsTimer();
	TimeLeftToTriggerSuperBeatWing = DataPtr_->SuperWingBeatTiming;

	OnWingBeatCalled.Broadcast();


}

void UPFWingBeatAbility::GetAverageInputValue()
{
	float RightInputDiff = FMathf::Abs(PreviousInputRightRegistered_ - 1);
	float LeftInputDiff = FMathf::Abs(PreviousInputLeftRegistered_ - 1);
	
	AverageInputValue_ = (RightInputDiff + LeftInputDiff)/2;
}

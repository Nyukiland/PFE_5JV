#include "Ability/PFWingBeatAbility.h"

#include "MathUtil.h"
#include "Ability/Data/PFDiveAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);
	
	TimeUntilNextInputRegistration -= deltaTime;
		
	if(TimeUntilNextInputRegistration <= 0.f)
	{
		PreviousInputRightRegistered_ = PreviousInputRight_;
		PreviousInputLeftRegistered_ = PreviousInputLeft_;
		InputRightRegistered_ = InputRight_;
		InputLeftRegistered_ = InputLeft_;
		TimeUntilNextInputRegistration = DataPtr_->DelayBetweenInputRegistrations;
	}

	TimeLeftToObtainSuperBeatWing -= deltaTime;
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
	TimeLeftToObtainSuperBeatWing = DataPtr_->SuperWingBeatTiming;
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

	if (!DataPtr_ || !DataPtr_->SuperWingBeatMultiplier)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatMultiplier'"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->SuperWingBeatTiming)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data 'SuperWingBeatTiming'"));
		return;
	}

	bool IsSuperBeatWingActivated = (TimeLeftToObtainSuperBeatWing > 0.f);
	UE_LOG(LogTemp, Warning, TEXT("TimeLeftToObtainSuperBeatWing : %f"),TimeLeftToObtainSuperBeatWing);
	if(IsSuperBeatWingActivated == true) UE_LOG(LogTemp, Error, TEXT("SuperBeatWingActivated"));
	
	GetAverageInputValue();
	// UE_LOG(LogTemp, Warning, TEXT("AverageInputValue : %f"), this->AverageInputValue_);
	
	float heightToGive = DataPtr_->ForceToGiveInHeight *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	if(IsSuperBeatWingActivated == true) heightToGive *= DataPtr_->SuperWingBeatMultiplier;
	PhysicResource_->AddForce(heightToGive * FVector::UpVector, true, false, DataPtr_->ForceToGiveInHeightDuration);
	
	float speedToGive = DataPtr_->ForceToGiveInVelocity *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	if(IsSuperBeatWingActivated == true) speedToGive *= DataPtr_->SuperWingBeatMultiplier;
	PhysicResource_->AddForwardForce(speedToGive, true, false, DataPtr_->ForceToGiveInHeightDuration);

	PhysicResource_->ResetGravityTimer();
	TimeLeftToObtainSuperBeatWing = DataPtr_->SuperWingBeatTiming;
}

void UPFWingBeatAbility::GetAverageInputValue()
{
	float RightInputDiff = FMathf::Abs(PreviousInputRightRegistered_ - 1);
	// UE_LOG(LogTemp, Warning, TEXT("RightInputDiff : %f = PreviousInputRightRegistered_ : %f - InputRightRegistered_ : %f"), RightInputDiff, PreviousInputRightRegistered_, InputRightRegistered_);
	float LeftInputDiff = FMathf::Abs(PreviousInputLeftRegistered_ - 1);
	// UE_LOG(LogTemp, Warning, TEXT("LeftInputDiff : %f = PreviousInputLeftRegistered_ %f - InputLeftRegistered_ %f"), LeftInputDiff, PreviousInputLeftRegistered_, InputLeftRegistered_);

	AverageInputValue_ = (RightInputDiff + LeftInputDiff)/2;
}

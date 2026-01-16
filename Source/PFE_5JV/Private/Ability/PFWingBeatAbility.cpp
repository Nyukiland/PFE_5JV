#include "Ability/PFWingBeatAbility.h"

#include "MathUtil.h"
#include "Ability/Data/PFDiveAbilityData.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);
	TimeSinceActivation += deltaTime;
	
	int RoundedTimeMultiplyBy100 = FMathf::Ceil(TimeSinceActivation*100);
	
	if (!DataPtr_ || !DataPtr_->PreviousInputRegistrationTime)
	{
		UE_LOG(LogTemp, Error, TEXT("[BeatWingAbility] Bad Set up on data PreviousInputRegistrationTime"));
		return;
	}
	
	int PreviousInputRegistrationTimeMultiplyBy100 = DataPtr_->PreviousInputRegistrationTime*100;
	
	if((RoundedTimeMultiplyBy100 % PreviousInputRegistrationTimeMultiplyBy100) == 0)
	{
		PreviousInputRightRegistered_ = PreviousInputRight_;
		PreviousInputLeftRegistered_ = PreviousInputLeft_;
		InputRightRegistered_ = InputRight_;
		InputLeftRegistered_ = InputLeft_;
	}
}

void UPFWingBeatAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResource_ = CastChecked<UPFPhysicResource>
	(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));
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
	
	GetAverageInputValue();
	// UE_LOG(LogTemp, Warning, TEXT("AverageInputValue : %f"), this->AverageInputValue_);
	
	float heightToGive = DataPtr_->ForceToGiveInHeight *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	PhysicResource_->AddForce(heightToGive * FVector::UpVector, false);
	
	float speedToGive = DataPtr_->ForceToGiveInVelocity *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	PhysicResource_->AddForwardForce(speedToGive, false);
}

void UPFWingBeatAbility::GetAverageInputValue()
{
	float RightInputDiff = FMathf::Abs(PreviousInputRightRegistered_ - 1);
	// UE_LOG(LogTemp, Warning, TEXT("RightInputDiff : %f = PreviousInputRightRegistered_ : %f - InputRightRegistered_ : %f"), RightInputDiff, PreviousInputRightRegistered_, InputRightRegistered_);
	float LeftInputDiff = FMathf::Abs(PreviousInputLeftRegistered_ - 1);
	// UE_LOG(LogTemp, Warning, TEXT("LeftInputDiff : %f = PreviousInputLeftRegistered_ %f - InputLeftRegistered_ %f"), LeftInputDiff, PreviousInputLeftRegistered_, InputLeftRegistered_);

	AverageInputValue_ = (RightInputDiff + LeftInputDiff)/2;
}

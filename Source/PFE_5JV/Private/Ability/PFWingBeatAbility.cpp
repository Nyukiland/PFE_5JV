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

	// if the timer is in timing range for trigger the SuperWingBeat :
	if(SuperBeatWingTimer_ >= SuperWingBeatMinTiming_ && SuperBeatWingTimer_ <= SuperWingBeatMaxTiming_)
	{
		bIsSuperBeatWingPossible_ = true;
		VisualResourcePtr_->ChangeMeshMaterial(0, EStateMaterial::ESM_SuperWingBeatPossible);
	}
	// if the right timing is over :  
	else if(SuperBeatWingTimer_ > SuperWingBeatMaxTiming_)
	{
		SuperBeatWingTimer_ = -1.f; // Deactivate timer 
		bIsSuperBeatWingPossible_ = false; 
		VisualResourcePtr_->ChangeMeshMaterial(0, EStateMaterial::ESM_Normal); 
	}
	// if the timer is just launched but not in SuperWingBeatTimer yet : 
	else if(SuperBeatWingTimer_ >= 0.f && SuperBeatWingTimer_ < SuperWingBeatMinTiming_)
	{
		bIsSuperBeatWingPossible_ = false;
		if(VisualResourcePtr_->GetMeshMaterial(0) == EStateMaterial::ESM_SuperWingBeatPossible)
		{
			// GEngine->AddOnScreenDebugMessage();
			VisualResourcePtr_->ChangeMeshMaterial(0, EStateMaterial::ESM_Normal); 
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
	VisualResourcePtr_ = ownerObj->GetStateComponent<UPFVisualResource>();

	
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
	// Debug Data :
	HeightAtWingBeatBeginning_ = Owner->ForwardRootPtr->GetComponentLocation().Z;;
	TimeEffectiveHeightCalculus_ = 3.f;
	if(bIsSuperBeatWingPossible_) GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("SuperBeatWing Triggered")));
	
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
		
	// Give the average input value to calculate the wingbeat power : 
	GetAverageInputValue();
	
	float HeightToGive = DataPtr_->ForceToGiveInHeight *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);
	if(bIsSuperBeatWingPossible_ == true) HeightToGive *= DataPtr_->SuperWingBeatHeightMultiplier;
	PhysicResourcePtr_->AddForce(HeightToGive * FVector::UpVector, true, false, DataPtr_->ForceToGiveInHeightDuration);

	// Debug data :
	MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	
	float SpeedToGive = DataPtr_->ForceToGiveInVelocity *
		DataPtr_->WingBeatAccelerationBasedOnAverageInputValueCurve->GetFloatValue(AverageInputValue_);

	float maxForce = DataPtr_->MaxWingBeatForce;
	if(bIsSuperBeatWingPossible_ == true)
	{
		maxForce = DataPtr_->MaxSuperWingBeatForce;
		SpeedToGive *= DataPtr_->SuperWingBeatVelocityMultiplier;
		
	}
	float ForwardVelocity = PhysicResourcePtr_->CurrentForwardVelo_.Length();
	if(ForwardVelocity <= maxForce)
	{
		float toAdd = FMath::Min(0, maxForce - (ForwardVelocity + SpeedToGive));
		PhysicResourcePtr_->AddForwardForce(toAdd, false);
	}

	PhysicResourcePtr_->ResetPhysicsTimer();
	
	// Trigger timer for SuperBeatWing :
	SuperBeatWingTimer_ = 0.f;

	OnWingBeatCalled.Broadcast();
}

void UPFWingBeatAbility::GetAverageInputValue()
{
	float RightInputDiff = FMathf::Abs(PreviousInputRightRegistered_ - 1);
	float LeftInputDiff = FMathf::Abs(PreviousInputLeftRegistered_ - 1);
	
	AverageInputValue_ = (RightInputDiff + LeftInputDiff)/2;
}

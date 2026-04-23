#include "Ability/PFWingBeatAbility.h"

#include "MathUtil.h"
#include "Ability/PFDiveAbility.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/PFVisualResource.h"
#include "StateMachine/PFPlayerCharacter.h"

class UPFVisualResource;

FString UPFWingBeatAbility::GetInfo_Implementation()
{
	FString text = TEXT("<hb>WingBeat:</>");
	text += TEXT("\n <b>CurrentHeight: </>") + FString::Printf(TEXT("%f"), CurrentHeight_);
	text += TEXT("\n <b>Current Rotation: </>") + FString::Printf(TEXT("%f"), CurrentRot_);
	text += TEXT("\n <b>HeightAtWingBeatBeginning: </>") + FString::Printf(TEXT("%f"), HeightAtWingBeatBeginning_);
	text += TEXT("\n <b>MaxHeightGain: </>") + FString::Printf(TEXT("%f"), MaxHeightGain_);

	return text;
}

void UPFWingBeatAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();
	DiveAbilityPtr_ = ownerObj->GetStateComponent<UPFDiveAbility>();
}

void UPFWingBeatAbility::ComponentEnable_Implementation()
{
	Super::ComponentEnable_Implementation();

	CurrentRot_ = 0;
}

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!PhysicResourcePtr_ && !DiveAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] missing component"));
		return;
	}

	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}

	if (DiveAbilityPtr_->IsDiving())
	{
		CurrentRot_ = 0;
		WingBeatInARowTimer_ = 100;
		return;
	}

	if (CurrentRot_ > 0)
	{
		WingBeatInARowTimer_ += deltaTime;
		
		PhysicResourcePtr_->SetPitchRotationVisual(CurrentRot_, -3);

		if (WingBeatInARowTimer_ > DataPtr_->TimerReset)
		{
			CurrentRot_ = 0;
		}
	}
}

void UPFWingBeatAbility::WingBeat()
{
	if (!PhysicResourcePtr_ && !DiveAbilityPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] missing component"));
		return;
	}

	if (!DataPtr_ || !DataPtr_->WingBeatAddForceBasedOnTimeCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}

	// Velocity
	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	if (forwardVelo <= DataPtr_->MaxVelocityWingBeatVelocity)
	{
		float toAdd = DataPtr_->WingBeatForce;
		float veloWithAdded = forwardVelo + toAdd;
		float veloDiff = DataPtr_->MaxVelocityWingBeatVelocity - forwardVelo;
		toAdd = DataPtr_->MaxVelocityWingBeatVelocity > veloWithAdded ? toAdd : veloDiff;
		
		PhysicResourcePtr_->AddForwardVelocity(toAdd, true, true,
			DataPtr_->WingBeatTimeForceGiven,DataPtr_->WingBeatAddForceBasedOnTimeCurvePtr);
	}

	// Actual Rotation
	WingBeatInARowTimer_ = 0;

	if (CurrentRot_ == 0)
	{
		if (PhysicResourcePtr_->CurrentPitchValue_ <= 0)
		{
			CurrentRot_ = DataPtr_->RotationPerClap;
		}
		else
		{
			CurrentRot_ = PhysicResourcePtr_->CurrentPitchValue_ + DataPtr_->RotationPerClap;
		}
	}
	else
	{
		CurrentRot_ += 	DataPtr_->RotationPerClap;
	}

	CurrentRot_ = FMath::Clamp(CurrentRot_, 0, DataPtr_->MaxWingBeatRotation);

	PhysicResourcePtr_->ResetPhysicsTimer();
	OnWingBeatCalled.Broadcast();
}

bool UPFWingBeatAbility::IsCurrentlyGoingUp()
{
	return CurrentRot_ != 0;
}

float UPFWingBeatAbility::GetCurrentWingBeatPercentage()
{
	if (!IsCurrentlyGoingUp())
	{
		return 0;
	}

	return CurrentRot_ / DataPtr_->MaxWingBeatRotation;
}

void UPFWingBeatAbility::DebugHeight()
{
	if (Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight_)
	{
		MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	}
	CurrentHeight_ = Owner->ForwardRootPtr->GetComponentLocation().Z;
}

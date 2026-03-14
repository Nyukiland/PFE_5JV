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

	return text;
}

void UPFWingBeatAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = ownerObj->GetStateComponent<UPFPhysicResource>();
}

void UPFWingBeatAbility::ComponentEnable_Implementation()
{
	Super::ComponentEnable_Implementation();

	WingBeatInARowCount = -1;
}

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!DataPtr_ || !DataPtr_->RotationCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}

	if (WingBeatInARowCount > 0)
	{
		WingBeatInARowTimer += deltaTime;
		
		float rotationToGive = DataPtr_->MaxRotationValue;
		rotationToGive *= DataPtr_->RotationCurvePtr->GetFloatValue(WingBeatValue01);
		PhysicResourcePtr_->SetPitchRotationVisual(rotationToGive, -3);

		if (WingBeatInARowTimer > DataPtr_->TimerReset)
		{
			WingBeatInARowCount = -1;
		}
	}
}

void UPFWingBeatAbility::WingBeat()
{
	if (!DataPtr_ || !DataPtr_->WingBeatAddForceBasedOnTimeCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}

	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	if (forwardVelo <= DataPtr_->MaxVelocityWingBeatVelocity)
	{
		float toAdd = DataPtr_->WingBeatForce;
		float veloWithAdded = forwardVelo + toAdd;
		float veloDiff = DataPtr_->MaxVelocityWingBeatVelocity - forwardVelo;
		toAdd = DataPtr_->MaxVelocityWingBeatVelocity > veloWithAdded ? toAdd : veloDiff;
		PhysicResourcePtr_->AddForwardVelocity(toAdd, true, true,
												DataPtr_->WingBeatTimeForceGiven, DataPtr_->WingBeatAddForceBasedOnTimeCurve);
	}

	WingBeatInARowTimer = 0;

	if (FMath::Abs(PhysicResourcePtr_->CurrentPitchValue_) < 1 || WingBeatInARowCount != -1)
	{
		WingBeatInARowCount++;
	}
	else
	{
		float pitchRatio = FMath::Abs(PhysicResourcePtr_->CurrentPitchValue_) / DataPtr_->MaxRotationValue;
		pitchRatio = FMath::Clamp(pitchRatio, 0.0f, 1.0f);
        
		WingBeatInARowCount = FMath::RoundToInt(pitchRatio * DataPtr_->MaxClapCount);
	}
	
	RecalculateWingBeat01();
	OnWingBeatCalled.Broadcast(WingBeatInARowCount);
}

void UPFWingBeatAbility::DebugHeight()
{
	if (Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight_)
	{
		MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	}
	CurrentHeight_ = Owner->ForwardRootPtr->GetComponentLocation().Z;
}

float UPFWingBeatAbility::RecalculateWingBeat01()
{
	WingBeatValue01 = static_cast<float>(WingBeatInARowCount) / static_cast<float>(DataPtr_->MaxClapCount);
	WingBeatValue01 = FMath::Clamp(WingBeatValue01, 0.0f, 1.0f);
	return WingBeatValue01;
}

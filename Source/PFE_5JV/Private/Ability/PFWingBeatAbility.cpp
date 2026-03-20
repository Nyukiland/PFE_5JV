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
	text += TEXT("\n <b>WingBeatInARowCount: </>") + FString::Printf(TEXT("%d"), WingBeatInARowCount);
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

	WingBeatInARowCount = -1;
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
		WingBeatInARowCount = 0;
		WingBeatInARowTimer = 100;
		return;
	}

	if (WingBeatInARowCount >= 0)
	{
		WingBeatInARowTimer += deltaTime;

		float rotationToGive = DataPtr_->RotationPerClap[WingBeatInARowCount];
		PhysicResourcePtr_->SetPitchRotationVisual(rotationToGive, -3);

		if (WingBeatInARowTimer > DataPtr_->TimerReset)
		{
			WingBeatInARowCount = -1;
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

	float forwardVelo = PhysicResourcePtr_->CurrentForwardVelocity_.Length();
	if (forwardVelo <= DataPtr_->MaxVelocityWingBeatVelocity)
	{
		float toAdd = DataPtr_->WingBeatForce;
		float veloWithAdded = forwardVelo + toAdd;
		float veloDiff = DataPtr_->MaxVelocityWingBeatVelocity - forwardVelo;
		toAdd = DataPtr_->MaxVelocityWingBeatVelocity > veloWithAdded ? toAdd : veloDiff;
		PhysicResourcePtr_->AddForwardVelocity(toAdd, true, true,
												DataPtr_->WingBeatTimeForceGiven,
												DataPtr_->WingBeatAddForceBasedOnTimeCurvePtr);
	}

	WingBeatInARowTimer = 0;

	if (FMath::Abs(PhysicResourcePtr_->CurrentPitchValue_) < 1 || WingBeatInARowCount != -1)
	{
		WingBeatInARowCount++;
	}
	else
	{
		WingBeatInARowCount = FindClosestClapValue(PhysicResourcePtr_->CurrentPitchValue_);
	}

	WingBeatInARowCount = FMath::Clamp(WingBeatInARowCount, 0, DataPtr_->RotationPerClap.Num() - 1);
	
	PhysicResourcePtr_->ResetPhysicsTimer();
	OnWingBeatCalled.Broadcast(WingBeatInARowCount);
}

bool UPFWingBeatAbility::IsCurrentlyGoingUp()
{
	return WingBeatInARowCount != -1;
}

void UPFWingBeatAbility::DebugHeight()
{
	if (Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight_)
	{
		MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	}
	CurrentHeight_ = Owner->ForwardRootPtr->GetComponentLocation().Z;
}

float UPFWingBeatAbility::FindClosestClapValue(float pitch) const
{
	int validIndex = 0;
	float closestAngle = 1000;
	for (int i = 0; i < DataPtr_->RotationPerClap.Num(); i++)
	{
		float angle = FMath::Abs(DataPtr_->RotationPerClap[i] - pitch);

		if (angle < closestAngle)
		{
			closestAngle = angle;
			validIndex = i;
		}
		else
		{
			break;
		}
	}

	return validIndex;
}

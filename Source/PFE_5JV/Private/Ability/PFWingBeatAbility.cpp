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

	
	WingBeatInARowCount = 0;
}

void UPFWingBeatAbility::ComponentTick_Implementation(float deltaTime)
{
	Super::ComponentTick_Implementation(deltaTime);

	if (!DataPtr_ || !DataPtr_->RotationCurvePtr || !DataPtr_->VelocityToGiveCurvePtr)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}
	
	if (WingBeatInARowCount > 0)
	{
		WingBeatInARowTimer += deltaTime;

		float value01 = static_cast<float>(WingBeatInARowCount)/static_cast<float>(DataPtr_->MaxClapCount);
		value01 = FMath::Clamp(value01, 0.0f, 1.0f);

		float veloToGive = DataPtr_->MaxVelocityWingBeatVelocity;
		veloToGive *= DataPtr_->VelocityToGiveCurvePtr->GetFloatValue(value01);
		PhysicResourcePtr_->AddForwardVelocity(veloToGive);

		float rotationToGive = DataPtr_->MaxRotationValue;
		rotationToGive *= DataPtr_->RotationCurvePtr->GetFloatValue(value01);
		PhysicResourcePtr_->SetPitchRotationVisual(rotationToGive, -3);

		if (WingBeatInARowTimer > DataPtr_->TimerReset)
		{
			WingBeatInARowCount = -1;
		}
	}
}

void UPFWingBeatAbility::WingBeat()
{
	if (!DataPtr_)
	{
		UE_LOG(LogTemp, Error, TEXT("[WingBeatAbility] Bad set up on data"));
		return;
	}

	OnWingBeatCalled.Broadcast(WingBeatInARowCount);
	WingBeatInARowTimer = 0;
	WingBeatInARowCount++;
}

void UPFWingBeatAbility::DebugHeight()
{
	if(Owner->ForwardRootPtr->GetComponentLocation().Z > CurrentHeight_)
	{
		MaxHeightGain_ = Owner->ForwardRootPtr->GetComponentLocation().Z - HeightAtWingBeatBeginning_;
	}
	CurrentHeight_ = Owner->ForwardRootPtr->GetComponentLocation().Z;
}

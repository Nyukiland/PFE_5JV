#include "Ability/PFRollAbility.h"

#include "Kismet/GameplayStatics.h"
#include "Resource/PFPhysicResource.h"
#include "StateMachine/PFPlayerCharacter.h"

void UPFRollAbility::ComponentInit_Implementation(APFPlayerCharacter* ownerObj)
{
	Super::ComponentInit_Implementation(ownerObj);

	PhysicResourcePtr_ = CastChecked<UPFPhysicResource>
		(Owner->GetStateComponent(UPFPhysicResource::StaticClass()));

	VisualResourcePtr_ = CastChecked<UPFVisualResource>
		(Owner->GetStateComponent(UPFVisualResource::StaticClass()));
}

void UPFRollAbility::ComponentEnable_Implementation()
{
	Super::ComponentEnable_Implementation();

	// Need to think this through but it seems like i should not reset
	// I will check specifically which one should be reset
	
	PressTimeRight_ = 0;
	PressTimeLeft_ = 0;
	// TimerRoll_ = 10;
	// TimerPreRoll_ = -1;
	// RotationDir_ = 0;
	// bIsRollComplete_ = true;
}

void UPFRollAbility::TryCallRollRight(float inputValue, int rotationSide)
{
	if (!Data_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Roll] Bad Set up on Data"));
		return;
	}
	
	if (!bIsRollComplete_)
		return;

	if (inputValue < Data_->ThresholdInput)
	{
		bIsDownRight_ = false;
		return;
	}

	if (bIsDownRight_)
		return;

	bIsDownRight_ = true;
	float pressTime = UGameplayStatics::GetRealTimeSeconds(Owner->GetWorld());
	
	if (PressTimeRight_ == 0 ||
		pressTime - PressTimeRight_ > Data_->RollRepetitionTime)
	{
		PressTimeRight_ = pressTime;
		return;
	}

	RotationDir_ += rotationSide;
	TimerPreRoll_ = 0;
}

void UPFRollAbility::TryCallRollLeft(float inputValue, int rotationSide)
{
	if (!Data_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Roll] Bad Set up on Data"));
		return;
	}
	
	if (!bIsRollComplete_)
		return;
	
	if (inputValue < Data_->ThresholdInput)
	{
		bIsDownLeft_ = false;
		return;
	}

	if (bIsDownLeft_)
		return;

	bIsDownLeft_ = true;

	float pressTime = UGameplayStatics::GetRealTimeSeconds(Owner->GetWorld());
	
	if (PressTimeLeft_ == 0 ||
		pressTime - PressTimeLeft_ > Data_->RollRepetitionTime)
	{
		PressTimeLeft_ = pressTime;
		return;
	}

	RotationDir_ += rotationSide;
	TimerPreRoll_ = 0;
}

bool UPFRollAbility::PreRollCheck(float deltatTime)
{
	if (!Data_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Roll] Bad Set up on Data"));
		return false;
	}

	if (TimerPreRoll_ <= -1)
		return false;

	TimerPreRoll_ += deltatTime;

	if (TimerPreRoll_ < Data_->WaitBeforeCallingWhenValid)
		return false;

	if (RotationDir_ == 0)
		return false;

	TimerRoll_ = 0;
	bIsRollComplete_ = false;
	PrevQuat_ = VisualResourcePtr_->GetBirdVisualRotation().Quaternion();
	TimerPreRoll_ = -1;
	return true;
}

bool UPFRollAbility::Roll(float deltaTime)
{
	if (!Data_)
	{
		UE_LOG(LogTemp, Error, TEXT("[Roll] Bad Set up on Data"));
		return false;
	}

	if (TimerRoll_ >= Data_->RollDuration)
	{
		if (!bIsRollComplete_)
		{
			bIsRollComplete_ = true;
			RotationDir_ = 0;
		}

		return true;
	}

	TimerRoll_ += deltaTime;

	float value = FMath::Clamp(TimerRoll_ / Data_->RollDuration, 0, 1);

	FVector rightVector = ForwardRoot->GetRightVector();
	PhysicResourcePtr_->AddForce(Data_->RollForce * Data_->RollForceOverTimePtr->GetFloatValue(value) * rightVector * RotationDir_);

	float totalDegrees = Data_->RotationCount * 360.f * -RotationDir_;
	float currentDegrees = totalDegrees * value;

	VisualResourcePtr_->SetRollRotation(currentDegrees, -2);

	return false;
}

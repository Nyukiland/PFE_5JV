#pragma once

#include "CoreMinimal.h"
#include "Data/PFDiveAbilityData.h"
#include "Input/PFInputFilteringData.h"
#include "Resource/PFPhysicResource.h"
#include "Resource/PFVisualResource.h"
#include "StateMachine/StateComponent/PFAbility.h"
#include "PFDiveAbility.generated.h"

UCLASS(Abstract, Blueprintable)
class PFE_5JV_API UPFDiveAbility : public UPFAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float CurrentMedianValue_;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFDiveAbilityData> DataPtr_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dive")
	TObjectPtr<UPFInputFilteringData> DataInputPtr_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFPhysicResource> PhysicResourcePtr_;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFVisualResource> VisualResourcePtr_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	TObjectPtr<UPFHapticsResource> HapticsResource_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float InputRight_;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float InputLeft_;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dive")
	float HighestInput_;

	float TimerAutoDive_ = 0;
	
	bool bIsDivingStateGoingUp_;
	float SpeedBeforeDive_;
	float GoingUpTimer_ = 10;
	float MaxTimeGoingUp_ = 0;

	float RecordedPreviousInputRight_;
	float RecordedPreviousInputLeft_;
	float TimerInputRecording_;
	float TimerWaitToRollDive_;
	bool bIsDiveRollPreviousDivingState_;
	int DiveRollDirection;

	float ElapsedTime_;
	
public:
	virtual void ComponentInit_Implementation(APFPlayerCharacter* ownerObj) override;
	virtual void ComponentTick_Implementation(float deltaTime) override;
	virtual FString GetInfo_Implementation() override;

	virtual void ComponentDisable_Implementation() override;

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputLeft(float left);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void ReceiveInputRight(float right);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void Dive(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void AfterDiveGoingUp(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	void DiveVisual(float deltaTime);
	
	void DiveHaptics();

	UFUNCTION(BlueprintCallable, Category="Dive")
	void DiveRoll(float deltaTime);

	UFUNCTION(BlueprintCallable, Category="Dive")
	bool IsDiving();

	UFUNCTION(BlueprintCallable, Category="Dive")
	float GetDivingValue();
	
	UFUNCTION(BlueprintCallable, Category="AutoDive")
	void AutoDive(float deltaTime);
	
	UFUNCTION(BlueprintCallable, Category="AutoDive")
	bool AutoDiveComplete() const;

	UFUNCTION(BlueprintCallable, Category="AutoDive")
	bool AutoDiveRotationComplete() const;
	
private:
	void GetHighestValue();

	void DiveRollInputRecording(float deltaTime);
	void DiveRollCheck();
};
